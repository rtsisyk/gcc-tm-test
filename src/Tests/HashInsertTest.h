#ifndef HASHINSERTTEST_H
#define HASHINSERTTEST_H

#include "Test.h"
#include <Utils/HashMap.h>

class HashInsertTest: public NumbersTest {
public:
    typedef Utils::HashMap<int, int> MyMap;

    virtual void setup() {
        m_sharedMap.clear();

        // HACK: performance
        const size_t maxBuckets = 1048576;
        if (m_inputSize > maxBuckets) {
            m_sharedMap.reserve(maxBuckets);
        } else {
            m_sharedMap.reserve(m_inputSize);
        }
    }

    virtual void teardown() {
        m_sharedMap.clear();
    }

    virtual bool check() {
        std::vector<int> inputSorted(m_input);
        std::vector<int> resultSorted;
        resultSorted.reserve(inputSorted.size());

        for(MyMap::Iterator it = m_sharedMap.begin(); it != m_sharedMap.end(); it++) {
            resultSorted.push_back(it.key());
        }

        if (resultSorted.size() != inputSorted.size()) {
            return false;
        }

        std::sort(inputSorted.begin(), inputSorted.end());
        std::sort(resultSorted.begin(), resultSorted.end());

        for (size_t i = 0; i < inputSorted.size(); i++) {
            const int srcValue = inputSorted[i];
            const int hashValue = resultSorted[i];

            if (srcValue != hashValue) {
                return false;
            }
        }

        return true;
    }

protected:

    virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            BEGIN_CRITICAL_SECTION();
                m_sharedMap.insertMulti(m_input[i], 0);
            END_CRITICAL_SECTION();
        }
    }

    MyMap m_sharedMap;
};


#endif // HASHINSERTTEST_H
