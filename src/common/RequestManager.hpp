/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef REQUEST_MANAGER_HPP
#define REQUEST_MANAGER_HPP

#include <CL/sycl.hpp>

#include <boost/intrusive/list.hpp>
#include <boost/lockfree/spsc_queue.hpp>

#include <cassert>

#include "util/ErrorHandler.hpp"


namespace tasycl {

//! Struct that represents a TASYCL request
struct Request {
	typedef boost::intrusive::link_mode<boost::intrusive::normal_link> link_mode_t;
	typedef boost::intrusive::list_member_hook<link_mode_t> links_t;

	//! The event of the request
    sycl::event _event;

	//! Calling task
	TaskingModel::task_handle_t _taskHandle;

	//! Support for Boost intrusive lists
	links_t _listLinks;

	inline Request() :
		_taskHandle(nullptr)
	{
	}
};


//! Class that manages the TASYCL requests
class RequestManager {
private:
	typedef boost::lockfree::spsc_queue<Request*, boost::lockfree::capacity<63*1024> > add_queue_t;
	typedef boost::intrusive::member_hook<Request, typename Request::links_t, &Request::_listLinks> hook_option_t;
	typedef boost::intrusive::list<Request, hook_option_t> list_t;
    typedef sycl::info::event_command_status event_status_t;

	//! Fast queues to add requests concurrently
	static add_queue_t _addQueue;

	//! Lock to add requests
	static SpinLock _addQueueLock;

	//! List of pending requests
	static list_t _pendingRequests;

	//! \brief Add a TASYCL request
	//!
	//! \param request The request to add
	static void addRequest(Request *request)
	{
		_addQueueLock.lock();
		while (!_addQueue.push(request)) {
			util::spinWait();
		}
		_addQueueLock.unlock();
	}

	//! \brief Add multiple TASYCL requests
	//!
	//! \param requests The requests to add
	static void addRequests(size_t count, Request *const requests[])
	{
		size_t added = 0;
		_addQueueLock.lock();
		do {
			added += _addQueue.push(requests+added, count-added);
		} while (added < count);
		_addQueueLock.unlock();
	}

public:
	//! \brief Generate a TASYCL request waiting for event completion
	//!
	//! \param event The queue to wait
	//! \param bind Whether should be bound to the current task
	static Request *generateRequest(sycl::event event, bool bind)
	{
		Request *request = Allocator<Request>::allocate();
		assert(request != nullptr);

        request->_event = event;

		// Bind the request to the calling task if needed
		if (bind) {
			TaskingModel::task_handle_t task = TaskingModel::getCurrentTask();
			assert(task != nullptr);

			request->_taskHandle = task;

			TaskingModel::increaseCurrentTaskEvents(task, 1);

			RequestManager::addRequest(request);
		}
		return request;
	}

	static void processRequest(Request *request)
	{
		assert(request != nullptr);

		TaskingModel::task_handle_t task = TaskingModel::getCurrentTask();
		assert(task != nullptr);

		request->_taskHandle = task;

		TaskingModel::increaseCurrentTaskEvents(task, 1);

		addRequest(request);
	}

	static void processRequests(size_t count, Request *const requests[])
	{
		assert(count > 0);
		assert(requests != nullptr);

		TaskingModel::task_handle_t task = TaskingModel::getCurrentTask();

		size_t nactive = 0;
		for (size_t r = 0; r < count; ++r) {
			if (requests[r] != nullptr) {
				assert(requests[r]->_taskHandle == nullptr);
				requests[r]->_taskHandle = task;
				++nactive;
			}
		}
		TaskingModel::increaseCurrentTaskEvents(task, nactive);

		addRequests(count, requests);
	}

	static void checkRequests()
	{
		if (!_addQueue.empty()) {
			_addQueue.consume_all(
				[&](Request *request) {
					if (request != nullptr)
						_pendingRequests.push_back(*request);
				}
			);
		}
    
        event_status_t eret;
		auto it = _pendingRequests.begin();
		while (it != _pendingRequests.end()) {
			Request &request = *it;
                
            eret = request._event.get_info<sycl::info::event::command_execution_status>();
            if (eret != sycl::info::event_command_status::complete &&
                eret != sycl::info::event_command_status::running &&
                eret != sycl::info::event_command_status::submitted  &&
                (int)eret != 3 /* FIX WHILE https://github.com/intel/llvm/issues/9099 IS FIXED */) {
                ErrorHandler::fail("Failed in event.get_info<sycl::info::event::command_execution_status>()");
            } else if (eret == sycl::info::event_command_status::complete) {
                assert(request._taskHandle != nullptr);
				TaskingModel::decreaseTaskEvents(request._taskHandle, 1);

				Allocator<Request>::free(&request);

				it = _pendingRequests.erase(it);
				continue;
            }
			++it;
		}
	}
};

} // namespace tasycl

#endif // REQUEST_MANAGER_HPP
