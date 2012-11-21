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

#ifndef LISTINSERTTEST_H
#define LISTINSERTTEST_H

#include "Test.h"
#include <Utils/LinkedList.h>

class ListInsertTest: public NumbersTest {
public:
    typedef Utils::LinkedList<int> MyList;

    virtual void setup() {
        m_sharedList.clear();
    }

    virtual void teardown() {
        m_sharedList.clear();
    }

    virtual bool check() {
        std::vector<int> inputSorted(m_input);
        std::vector<int> resultSorted;
        resultSorted.reserve(inputSorted.size());

        for(MyList::Iterator it = m_sharedList.begin(); it != m_sharedList.end(); it++) {
            resultSorted.push_back(it.value());
        }

        if (resultSorted.size() != inputSorted.size()) {
            return false;
        }

        std::sort(inputSorted.begin(), inputSorted.end());
        std::sort(resultSorted.begin(), resultSorted.end());

        for (size_t i = 0; i < inputSorted.size(); i++) {
            const int srcValue = inputSorted[i];
            const int listValue = resultSorted[i];

            if (srcValue != listValue) {
                return false;
            }
        }

        return true;
    }

protected:

    virtual void worker(size_t start, size_t end) {
        for(size_t i = start; i < end; i++) {
            bool back = (rand() % 2);
            BEGIN_CRITICAL_SECTION();
                if (back) {
                    m_sharedList.pushBack(m_input[i]);
                } else {
                    m_sharedList.pushFront(m_input[i]);
                }
            END_CRITICAL_SECTION();
        }
    }

    MyList m_sharedList;
};


#endif // LISTINSERTTEST_H
