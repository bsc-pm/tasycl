/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#include <TASYCL.hpp>

#include "common/Environment.hpp"
#include "common/TaskingModel.hpp"
#include "common/util/ErrorHandler.hpp"

using namespace tasycl;

#pragma GCC visibility push(default)

void
tasyclWaitRequestAsync(tasyclRequest *request)
{
	assert(request != nullptr);

	if (*request != TASYCL_REQUEST_NULL)
		RequestManager::processRequest((Request *) *request);

	*request = TASYCL_REQUEST_NULL;
}

void
tasyclWaitallRequestsAsync(size_t count, tasyclRequest requests[])
{
	if (count == 0)
		return;

	assert(requests != nullptr);

	RequestManager::processRequests(count, (Request * const *) requests);

	for (size_t r = 0; r < count; ++r) {
		requests[r] = TASYCL_REQUEST_NULL;
	}
}

#pragma GCC visibility pop
