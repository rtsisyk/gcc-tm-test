#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <random>
#include <algorithm>
#include <thread>

struct ITest {    
    virtual void generate(size_t inputSize, size_t threadsCount) = 0;

    virtual void setup() = 0;
    virtual void runSequential() = 0;
    virtual void runThreaded() = 0;
    virtual void teardown() = 0;

    virtual bool check()  = 0;

    virtual ~ITest() {

    }
};

class AbstractTest: public ITest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount) {
        m_inputSize = inputSize;
        m_threadsCount = threadsCount;
    }

    virtual void setup() {
        // nothing
    }

    virtual void runSequential() = 0;
    virtual void runThreaded() = 0;

    virtual void teardown() {
        // nothing
    }

    virtual bool check()  = 0;

protected:
    size_t m_inputSize;
    size_t m_threadsCount;
    std::random_device m_rnd;
};



#endif // COMMON_H
