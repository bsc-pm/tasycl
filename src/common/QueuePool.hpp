/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef QUEUE_POOL_HPP
#define QUEUE_POOL_HPP

#include <CL/sycl.hpp>

#include <cassert>
#include <vector>

#include "TaskingModel.hpp"
#include "util/ErrorHandler.hpp"


namespace tasycl {

//! Class that manages the TASYCL queues
class QueuePool {
private:
	//! Array of queues
	static std::vector<sycl::queue> _queues;

public:
	//! \brief Initialize the pool of queues
	//!
	//! \param nqueues The number of queues to create
	static inline void initialize(size_t nqueues, bool shareContext=true, bool inOrderQueues=false)
	{
		assert(nqueues > 0);

        sycl::cpu_selector dev_selector;
		_queues.resize(nqueues);

        sycl::property_list q_prop;
        if(inOrderQueues){
            q_prop = {sycl::property::queue::in_order()};
        }

        if(!shareContext){
            for (size_t s = 0; s < nqueues; ++s) {
                sycl::queue q(dev_selector, q_prop);
                _queues[s] = q;
            }
        } else{
            sycl::queue q1(dev_selector);
            _queues[0] = q1;
            for (size_t s = 1; s < nqueues; ++s){
                sycl::queue q(q1.get_context(), dev_selector, q_prop);
                _queues[s] = q;
            }
        }

	}

	//! \brief Finalize the pool of queues
	static inline void finalize()
	{
        // Empty on purpose
	}

	//! \brief Get queue within pool
	//!
	//! \param queueId The queue identifier within the pool
	static inline sycl::queue getQueue(size_t queueId)
	{
		assert(queueId >= 0);
		assert(queueId < getNumberOfQueues());

		return _queues[queueId];
	}

    //! \brief Get number of queues in pool
    static inline short int getNumberOfQueues(){
        return _queues.size();
    }
};

} // namespace tasycl

#endif // QUEUE_POOL_HPP
