#ifndef VECTORINSERTTEST_H
#define VECTORINSERTTEST_H

#include "Test.h"

#include <Utils/Vector.h>

class ArrayInsertTest: public NumbersTest {
public:
    typedef Utils::Vector<int> MyVector;

    virtual void setup() {
        //m_sharedVector.reserve(m_inputSize);
    }

    virtual void teardown() {
        m_sharedVector.clear();
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

    MyVector m_sharedVector;
};


#endif // VECTORINSERTTEST_H
