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
