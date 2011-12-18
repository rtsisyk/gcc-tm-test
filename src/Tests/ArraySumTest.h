#ifndef ARRAYSUMTEST_H
#define ARRAYSUMTEST_H

#include "Test.h"

class ArraySumTest: public NumbersTest {
public:
    virtual void setup() {
        m_sharedSum = 0;
    }

    virtual bool check() {
        size_t refSum = 0;
        for(size_t i = 0; i < m_inputSize; i++) {
            refSum += m_input[i];
        }

        return (refSum == m_sharedSum);
    }

protected:
    virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            const int val = m_input[i];
            BEGIN_CRITICAL_SECTION();
                m_sharedSum += val;
            END_CRITICAL_SECTION();
        }
    }

    size_t m_sharedSum;
};


#endif // ARRAYSUMTEST_H
