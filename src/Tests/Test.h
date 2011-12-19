#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <random>
#include <algorithm>
#include <thread>
#ifdef LOCKTYPE_MUTEX
#include <mutex>
#endif

#include "../Common.h"

struct ITest {
    virtual void generate(size_t inputSize, size_t threadsCount) = 0;

    virtual void setup() = 0;
    virtual void run() = 0;
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

    virtual void run() = 0;

    virtual void teardown() {
        // nothing
    }

    virtual bool check()  = 0;

protected:
    size_t m_inputSize;
    size_t m_threadsCount;
    std::random_device m_rnd;
};

class NumbersTest: public AbstractTest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount)
    {
        AbstractTest::generate(inputSize, threadsCount);
        m_input.resize(m_inputSize);
        for(size_t i = 0; i < m_inputSize; i++) {
            m_input[i] = m_rnd() % inputSize;
        }

        const size_t keysPerThread = m_inputSize / m_threadsCount;
        m_ranges.resize(m_threadsCount);
        size_t currentKey = 0;
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            m_ranges[threadId].first = currentKey;
            currentKey += keysPerThread;
            m_ranges[threadId].second = currentKey;
        }

        m_ranges[m_threadsCount-1].second = m_inputSize;
    }

    virtual void run() {
        std::vector<std::thread> threads;
        threads.resize(m_threadsCount);

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId] = std::thread(std::bind( &NumbersTest::worker, this,
                                                      m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
    }

protected:
#ifdef LOCKTYPE_MUTEX
    std::mutex lock;
#endif

    virtual void worker(size_t start, size_t end) = 0;

    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
};


#endif // COMMON_H
