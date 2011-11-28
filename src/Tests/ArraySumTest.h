#ifndef ARRAYSUMTEST_H
#define ARRAYSUMTEST_H

#include "Test.h"

class ArraySumTest: public AbstractTest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount) {
        AbstractTest::generate(inputSize, threadsCount);

        m_input.resize(m_inputSize);
        for(size_t i = 0; i < m_inputSize; i++) {
            m_input[i] = m_rnd() % 1000;
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

    virtual void setup() {
        m_sharedSum = 0;
    }

    virtual void runSequential() {
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            workerSequential(m_ranges[threadId].first, m_ranges[threadId].second);
        }
    }

    virtual void runThreaded() {
        std::vector<std::thread> threads;
        threads.resize(m_threadsCount);

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId] = std::thread(std::bind( &ArraySumTest::workerThreaded, this,
                                                      m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
    }

    virtual bool check() {
        size_t refSum = 0;
        for(size_t i = 0; i < m_inputSize; i++) {
            refSum += m_input[i];
        }

        return (refSum == m_sharedSum);
    }

protected:
    virtual void workerThreaded(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            const int val = m_input[i];
            __transaction_atomic {
                m_sharedSum += val;
            }
        }
    }

    virtual void workerSequential(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            const int val = m_input[i];
            m_sharedSum += val;
        }
    }



    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
    size_t m_sharedSum;
};


#endif // ARRAYSUMTEST_H
