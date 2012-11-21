## GCC Transaction Memory Test ##

##<a name="Overview">Overview</a>##

**GCC TM Test** is a set of various examples and tests demonstrating
Software Transaction Memory (STM) usage in GNU GCC 4.7+.

The package contains a simple benchmark of basic data structures in the
multithreaded environment, comparing performance of Transaction Memory
and traditional synchronization mechanisms .

##<a name="Project_Status">Project Status</a>##

Code was written by me during doing an experimental work in the my university. 
The _Experiment_ has been finished and the code has been published as is. It is
not maintained anymore. I hope that the code will be useful for developers who
want try to use TM technology from academic world in their real-life products.

##<a name="Transaction_Memory">Transactional Memory</a>##

In computer science, software transactional memory (STM) is a concurrency 
control mechanism analogous to database transactions for controlling access to 
shared memory in concurrent computing. It is an alternative to lock-based 
synchronization. A transaction in this context is a piece of code that executes 
a series of reads and writes to shared memory. These reads and writes logically 
occur at a single instant in time; intermediate states are not visible to other 
(successful) transactions. STM has recently been the focus of intense research 
and support for practical implementations is growing.

Check the [article in Wikipedia](http://en.wikipedia.org/wiki/Software_transactional_memory).

##<a name="Transaction_Memory_GCC">Transactional Memory in GCC</a>##

Sometimes theory meets practice. An experimental support for STM was introduced
in GCC 4.7.

Check the [article on GCC web-site](http://gcc.gnu.org/wiki/TransactionalMemory).

Transactions usage in C/C++ is pretty simple:
<pre>
__transaction_atomic {
    /* this block of operations is executed atomically */
}
</pre>

GCC implementation of transactional memory is transparent to the program and
most of it resides in a runtime library (libitm in GCC). There is various
implementations of the runtime:

* __libitm__ -- Intel Transaction Memory runtime. Shipped  with an experimental
    version of Intel C++ Compiler (ITM API was initially designed by Intel).
* __GNU libitm__ -- GNU implementation. Shipped with regular GCC package.
* __tinySTM__ -- fast implementation by 
        [Velox Project](http://www.velox-project.eu/).


We especially didn't used hard-core manual optimization, suitable only for TM.
We instead applied "naive" approach where synchronizations are applied only at
upper level, synchronizing entire modification operation.

    for(size_t i = start; i < end; i++) {
        BEGIN_CRITICAL_SECTION();
            m_sharedRedBlackTree.insertMulti(m_input[i]);
        END_CRITICAL_SECTION();
    }


All data structures were written from a scratch, because current GCC TM frontend
do not know how to test methods from external libraries for side effects and
detect when these methods are pure and when are not pure.

##<a name="Installation">Installation</a>##

###Dependencies Compilation####

You need:
    
* GNU GCC *version 4.7 or later*
* TinySTM, *version 1.0.0 or later*

If you favorite distro has required packages you can just use supplied versions
and skip next instructions.

Our `contrib` directory provides all required build scripts for building both
GCC and tinySTM.

    cd contrib; make tinystm; make gcc-trunk

Packages will be fetched from SCM and compiled in `contrib` directory.

###Package Compilation####
    make
    ...
    ls tester*
    tester-mutex  tester-onethread  tester-tm-gnu  tester-tm-tiny

##<a name="Usage">Usage</a>##

There is four versions of __tester__ application:

* __tester-onethread__ - single-threaded version
* __tester-mutex__ - version with std::mutex from C++1x standard
* __tester-tm-gnu__ - TM version with GNU runtime
* __tester-tm-tiny__ - TM version with TinySTM runtime

To run __tester__ you need `tests.cfg` configuration file 
(see an example in `tests.cfg-sample`):

    # Name of test, number of threads, input size, repeat count 
    ArraySumTest 2 1000000 3
    ArrayInsertTest 2 1000000 3
    ListInsertTest 2 100000 3
    TreeInsertTest 2 200000 3
    TreeRemoveTest 2 200000 3

Run all versions sequencially using `make run` command:

    roman@home:~/GCC-TM-Test$ make run
    TinySTM-ABI v1.0.3 using TinySTM 1.0.3.
    Tester compiled with LOCKTYPE_TM
    Wating for configuration data from stdin...
    Test: HashInsertTest
    Threads count: 4
    Input size: 100000
    Repeat count: 5
    Run... OK 46.611 ms, 2146 ops/s
    Run... OK 32.221 ms, 3104 ops/s
    Run... OK 30.314 ms, 3299 ops/s
    Run... OK 30.856 ms, 3241 ops/s
    Run... OK 29.010 ms, 3448 ops/s
    > HashInsertTest TM OK 100000 4 5 33.802 2959
    ...

##<a name="Results">Results</a>##

Some test results are available in `doc` folder.

[My PDF with graphs](http://github.com/rtsisyk/gcc-tm-test/blob/master/doc/tests.pdf).

__References:__

* [Transactional Memory in GCC](http://gcc.gnu.org/wiki/TransactionalMemory)
* [Software transactional memory](http://en.wikipedia.org/wiki/Software_transactional_memory)
in Wikipedia

__Authors:__ Roman Tsisyk ([`roman@tsisyk.com`](mailto:roman@tsisyk.com))
([`http://roman.tsisyk.com/`](http://roman.tsisyk.com/))

__Copyright:__ (c) 2011 Roman Tsisyk

Unfortunately, the code was published only a year after the original research 
due my time restrictions.
