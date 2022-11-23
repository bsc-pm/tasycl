/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#include <TASYCL.hpp>

#include "common/QueuePool.hpp"
#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"
#include "common/util/ErrorHandler.hpp"

using namespace tasycl;

#pragma GCC visibility push(default)

void
tasyclCreateQueues(size_t count=TaskingModel::getNumCPUs(), bool share_context)
{
	assert(count > 0);

	QueuePool::initialize(count, share_context);
}

void
tasyclDestroyQueues()
{
	QueuePool::finalize();
}

void
tasyclGetQueue(sycl::queue *queue, short int queueId /*= TASYCL_QUEUE_ID_DEFAULT*/)
{
	assert(queue != nullptr);
    if(queueId == TASYCL_QUEUE_ID_DEFAULT){
        queueId = TaskingModel::getCurrentCPU() % QueuePool::getNumberOfQueues();
    }
    assert(queueId >= 0);
    assert(queueId < QueuePool::getNumberOfQueues());

	*queue = QueuePool::getQueue(queueId);
}

void
tasyclReturnQueue(sycl::queue)
{
}

void
tasyclSynchronizeEventAsync(sycl::event e)
{
	RequestManager::generateRequest(e, true);
}

#pragma GCC visibility pop
