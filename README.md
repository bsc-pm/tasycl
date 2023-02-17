# Task-Aware SYCL Library

## Documentation and programming model
The Task-Aware SYCL (TASYCL) library extends the functionality of the SYCL programming model by providing new mechanisms for improving the interoperability between parallel task-based programming models, such as OpenMP and OmpSs-2, and the offloading of kernels to devices via SYCL. The library allows the efficient offloading of SYCL operations (kernels, memory copy, etc.) from concurrent tasks in an asynchronous manner. TASYCL manages most of the low-level aspects, so the developers can focus in exploiting the parallelism os the application.

The TASYCL library contains functions that are designed to bind task completion to the completion of SYCL operations in an asynchronous way, as a means to increase concurrency between different tasks and have interoperability between both programming models.

The idea behind the library is to let the tasks inside a taskified application to offload multiple SYCL operations into a given set of SYCL queues, and then these tasks should bind their completion to the finalization of their SYCL operations in an asynchronous way. All of this happens without blocking the host CPUs while the SYCL operations are running on the device, so this allows host CPUs to execute other ready tasks meanwhile. In general, tasks will use standard SYCL functions to offload operations and use TASYCL functions to bind and "synchronize" the task with the SYCL operations completion.

The library is initialized by calling tasyclInit. The TASYCL library is finalized by calling tasyclFinalize. In TASYCL applications it may be necessary to use multiple queues, as multiple tasks may want to submit operations on different SYCL queues concurrently (e.g. when using various devices, and have one queue per device). For this reason, the TASYCL library generates a pool of queues that can be used by the tasks during the execution of the application. This pool can be created with the function ```tasyclCreateQueues(size_t count, bool share_context)```, which if no parameters are received will create as many queues as cores available and all queues will share their context so that they can share data by default. The queue pool must be destroyed before the finalization of the library with ```tasyclDestroyQueues()```.
When a task wants to offload an operation to the SYCL device, it can retrieve one of these queues for exclusive use until it is returned. When the queue is not needed anymore, the task can return it to the pool. The functions corresponding to these actions are the following:

```
tasyclGetQueue(sycl::queue *queue, short int queueId = TASYCL_QUEUE_ID_DEFAULT);
tasyclReturnQueue(sycl::queue queue);
```

By default in ```tasyclGetQueue``` if no parameter ```queueId``` is given, the library will assign any queue it see fit.

When operations have been offloaded from a task, it may be useful for the task to not end until these operations have completed. For example, a SYCL operation that updates an array may be inside a task that another tasks depends on, so the current task needs to wait until the SYCL operation ends before it finishes and the second task begins. That's when the function ```tasyclSynchronizeEventAsync(sycl::event event)``` comes to use. Given the asynchronous nature of SYCL operations, this function allows the task to synchronize with the event returned by the SYCL operation, but in an asynchronous manner. It binds the completion of the task to the finalization of the submitted operation, by querying the sycl event received as parameter. This way the calling task will be able to continue and finish its execution but will delay its full completion (and the release of data dependencies) until all the events finalize.

Finally, inside the header file src/include/TASYCL.hpp, it can be found a brief explanation of every function in the library and their functionality, which can be used as a "cheat sheet". 

## Building and installing
TASYCL uses the standard GNU automake and libtool toolchain. When cloning from a repository, the building environment must be prepared executing the bootstrap script:

```
$ ./bootstrap
```

Then, to configure, compile and install the library execute the following commands:

```
$ CC=icx CXX=icpx ./configure --prefix=$INSTALL_PREFIX --with-sycl-include=$SYCL_INCLUDE
$ make
$ make install
```

where $INSTALL_PREFIX is the directory into which to install TASYCL, and $SYCL_INCLUDE is the path where CL/sycl.hpp is found. There is  another optional configuration flag, which may be useful if your SYCL implementation is not Intel's DPC++:

- `--with-sycl` : Specifies the path of the SYCL implementation.

Once TASYCL is built and installed, e.g, in $TASYCL_HOME, the installation folder will contain the library in $TASYCL_HOME/lib (or $TASYCL_HOME/lib) and the header in $TASYCL_HOME/include.

## Requirements
In order to install the TASYCL library, the main requirements are the following:

- Automake, autoconf, libtool, make and a C and C++ compiler that supports C++17.
- A SYCL implementation. TASYCL has been tested with OneAPI 2023.0.
- Boost library 1.59 or greater.
- OmpSs-2 (version 2022.11 or greater).
