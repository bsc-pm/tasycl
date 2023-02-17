/*
	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)
*/

#include <TASYCL.hpp>

#include "common/Environment.hpp"

using namespace tasycl;

#pragma GCC visibility push(default)

void
tasyclInit()
{
    Environment::initialize();
}

void
tasyclFinalize()
{
	Environment::finalize();
}

#pragma GCC visibility pop
