#	This file is part of Task-Aware SYCL and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2022 Barcelona Supercomputing Center (BSC)

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=\
 $(syclinc) \
 $(BOOST_CPPFLAGS) -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER \
 -I$(top_srcdir)/src \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/include \
 -include "config.h" \
 $(tasycl_CPPFLAGS)

AM_CXXFLAGS=$(tasycl_CXXFLAGS)

AM_LDFLAGS=$(BOOST_LDFLAGS) -ldl

include_HEADERS= \
 src/include/TASYCL.hpp
pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources= \
 src/c/Initialization.cpp \
 src/c/Requests.cpp \
 src/c/Queues.cpp

common_sources = \
 src/common/Environment.cpp \
 src/common/TaskingModel.cpp

noinst_HEADERS = \
 src/common/Allocator.hpp \
 src/common/Environment.hpp \
 src/common/QueuePool.hpp \
 src/common/RequestManager.hpp \
 src/common/Symbol.hpp \
 src/common/TaskingModelAPI.hpp \
 src/common/TaskingModel.hpp \
 src/common/util/EnvironmentVariable.hpp \
 src/common/util/ErrorHandler.hpp \
 src/common/util/LockFreeQueue.hpp \
 src/common/util/SpinLock.hpp \
 src/common/util/Utils.hpp

lib_LTLIBRARIES = libtasycl.la

libtasycl_la_CPPFLAGS=$(AM_CPPFLAGS)
libtasycl_la_SOURCES=$(common_sources) $(c_api_sources)
