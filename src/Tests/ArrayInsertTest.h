/*
 * (с) 2011 Roman Tsisyk <roman@tsisyk.com>
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * 1. Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the
 * following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * <COPYRIGHT HOLDER> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef ARRAYINSERTTEST_H
#define ARRAYINSERTTEST_H

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
    virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            BEGIN_CRITICAL_SECTION();
                m_sharedVector.pushBack(m_input[i]);
            END_CRITICAL_SECTION();
        }
    }

    MyVector m_sharedVector;
};


#endif // ARRAYINSERTTEST_H
