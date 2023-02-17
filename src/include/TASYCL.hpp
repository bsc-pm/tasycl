/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#ifndef TASYCL_HPP
#define TASYCL_HPP

#include <CL/sycl.hpp>

#include <stddef.h>
#include <stdint.h>

#pragma GCC visibility push(default)

typedef void *tasyclRequest;

static const tasyclRequest TASYCL_REQUEST_NULL = NULL;
static const short int TASYCL_QUEUE_ID_DEFAULT = -1;
static const size_t TASYCL_QUEUES_SHARE_CONTEXT = 1;
static const size_t TASYCL_NQUEUES_AUTO = 0;

//! \brief Initialization
//!
//! Initializes the TASYCL environment
void
tasyclInit();

//! \brief Finalization
//!
//! Finalizes the TASYCL environment
void
tasyclFinalize();

//! \brief Initialization of the pool of queues
//!
//! Initializes the pool of queues with count queues
void
tasyclCreateQueues(size_t count, bool shareContext=true, bool inOrderQueues=false);

//! \brief Finalization of the pool of queues
void
tasyclDestroyQueues();

//! \brief Getting a queue
//!
//!  Gets a queue from the pool by passing a pointer to a previously declarated queue
//!  Gets queue number 'i'.
void
tasyclGetQueue(sycl::queue *queue, short int queueId=TASYCL_QUEUE_ID_DEFAULT);

//! \brief Returning a queue to the pool
void
tasyclReturnQueue(sycl::queue queue);

//! \brief Binding the calling task to a queue
//!
//! Asynchronous function, binds the completion of the calling task
//! to the finalization of the event passed as parameter
void
tasyclSynchronizeEventAsync(sycl::event e);

//! \brief Binding a request 
//!
//! Asynchronous and non-blocking operation, binds a request to the calling task
void
tasyclWaitRequestAsync(tasyclRequest *request);

//! \brief Bindig multiple requests
//!
//! Asynchronous and non-blocking operation, binds the calling task 
//! to count requests, all of them stored in "requests"
void
tasyclWaitallRequestsAsync(size_t count, tasyclRequest requests[]);

#pragma GCC visibility pop

#endif /* TASYCL_HPP */
