#ifndef TREEINSERTTEST_H
#define TREEINSERTTEST_H

#include "Test.h"

#include <Utils/TreeSet.h>

class TreeInsertTest: public AbstractTest {
public:
    typedef Utils::TreeSet<int> MySet;

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
        m_sharedSet.clear();
    }

    virtual void teardown() {
        m_sharedSet.clear();
    }

    virtual void runSequential() {
        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            worker(m_ranges[threadId].first, m_ranges[threadId].second);
        }
    }

    virtual void runThreaded() {
        std::vector<std::thread> threads;
        threads.resize(m_threadsCount);

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId] = std::thread(std::bind( &TreeInsertTest::worker, this,
                                                      m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
    }

    virtual bool check() {
        std::vector<int> inputSorted(m_input);

        size_t i = 0;
        std::sort(inputSorted.begin(), inputSorted.end());

        for (MySet::Iterator it = m_sharedSet.begin(); it != m_sharedSet.end(); it++) {
            const int srcKey = inputSorted[i];
            const int setKey = it.key();

            if (srcKey != setKey) {
                return false;
            }

            i++;
        }

        return true;
    }

protected:
   virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            __transaction_atomic {
                m_sharedSet.insertMulti(m_input[i]);
            }
        }
    }

    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
    MySet m_sharedSet;
};


#endif // TREEINSERTTEST_H
