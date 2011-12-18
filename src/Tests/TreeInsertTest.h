#ifndef TREEINSERTTEST_H
#define TREEINSERTTEST_H

#include "Test.h"

#include <Utils/TreeSet.h>

class TreeInsertTest: public NumbersTest {
public:
    typedef Utils::TreeSet<int> MySet;

    virtual void setup() {
        m_sharedSet.clear();
    }

    virtual void teardown() {
        m_sharedSet.clear();
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
            BEGIN_CRITICAL_SECTION();
                m_sharedSet.insertMulti(m_input[i]);
            END_CRITICAL_SECTION();
        }
    }

    MySet m_sharedSet;
};


#endif // TREEINSERTTEST_H
