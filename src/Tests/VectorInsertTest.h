#ifndef VECTORINSERTTEST_H
#define VECTORINSERTTEST_H

#include "Test.h"

#include <Utils/Vector.h>

class VectorInsertTest: public AbstractTest {
public:
    typedef Utils::Vector<int> MyVector;

    virtual void generate(size_t inputSize, size_t threadsCount)
    {
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
        //m_sharedVector.reserve(m_inputSize);
    }

    virtual void teardown() {
        m_sharedVector.clear();
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
            threads[threadId] = std::thread(std::bind( &VectorInsertTest::workerThreaded, this,
                                                      m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
    }

    virtual bool check() {
        std::vector<int> inputSorted(m_input);
        std::sort(inputSorted.begin(), inputSorted.end());
        std::sort(m_sharedVector.begin(), m_sharedVector.end());

        for (size_t i = 0; i < m_inputSize; i++) {
            if (inputSorted[i] != m_sharedVector[i]) {
                return false;
            }
        }

        return true;
    }

protected:
   virtual void workerSequential(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            m_sharedVector.pushBack(m_input[i]);
        }
    }

    virtual void workerThreaded(size_t start, size_t end) {
         for(size_t i = start; i < end; i++) {
             __transaction_atomic {
                 m_sharedVector.pushBack(m_input[i]);
             }
         }
     }

    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
    MyVector m_sharedVector;
};


#endif // VECTORINSERTTEST_H
