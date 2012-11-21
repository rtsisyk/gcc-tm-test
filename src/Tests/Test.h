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

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <random>
#include <algorithm>
#include <thread>
#ifdef LOCKTYPE_MUTEX
#include <mutex>
#endif

#include "../Common.h"

struct ITest {
    virtual void generate(size_t inputSize, size_t threadsCount) = 0;

    virtual void setup() = 0;
    virtual void run() = 0;
    virtual void teardown() = 0;

    virtual bool check()  = 0;

    virtual ~ITest() {

    }
};

class AbstractTest: public ITest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount) {
        m_inputSize = inputSize;
        m_threadsCount = threadsCount;
    }

    virtual void setup() {
        // nothing
    }

    virtual void run() = 0;

    virtual void teardown() {
        // nothing
    }

    virtual bool check()  = 0;

protected:
    size_t m_inputSize;
    size_t m_threadsCount;
    std::random_device m_rnd;
};

class NumbersTest: public AbstractTest {
public:
    virtual void generate(size_t inputSize, size_t threadsCount)
    {
        AbstractTest::generate(inputSize, threadsCount);
        m_input.resize(m_inputSize);
        for(size_t i = 0; i < m_inputSize; i++) {
            m_input[i] = m_rnd() % inputSize;
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

    virtual void run() {
        std::vector<std::thread> threads;
        threads.resize(m_threadsCount);

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId] = std::thread(std::bind( &NumbersTest::worker,
                    this, m_ranges[threadId].first, m_ranges[threadId].second));
        }

        for(size_t threadId = 0; threadId < m_threadsCount; threadId++) {
            threads[threadId].join();
        }
    }

protected:
#ifdef LOCKTYPE_MUTEX
    std::mutex lock;
#endif

    virtual void worker(size_t start, size_t end) = 0;

    std::vector<int> m_input;
    std::vector< std::pair<size_t, size_t> > m_ranges;
};


#endif // COMMON_H
