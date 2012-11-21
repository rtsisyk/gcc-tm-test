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
