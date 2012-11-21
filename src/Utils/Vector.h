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

#ifndef VECTOR_H
#define VECTOR_H

namespace Utils {

/**
 * Simple Vector implementation
 */
template<class ValueTypeParam>
class Vector {
public:
    typedef ValueTypeParam ValueType;
    typedef ValueType *Iterator;

    __attribute__((transaction_safe))
    Vector() {
        m_size = 0;
        m_capacity = 10;
        m_data = new ValueType[m_capacity];
    }

    __attribute__((transaction_safe))
    virtual ~Vector() {
        clear();
        delete [] m_data;
    }

    __attribute__((transaction_safe))
    Vector(const Vector& vec) {
        m_size = 0;
        m_capacity = 0;
        m_data = new ValueType[m_capacity];

        *this = vec;
    }

    __attribute__((transaction_safe))
    Vector& operator=(const Vector& vec) {
        resize(vec.size());
        for(size_t i = 0; i < vec.size(); i++) {
            m_data[i] = vec.m_data[i];
        }

        return *this;
    }

    Iterator begin() const {
        return m_data;
    }

    Iterator end() const {
        return (m_data + m_size);
    }

    void pushBack(const ValueType& val) {
        if (m_size == m_capacity) {
            reserve(2 * m_capacity);
        }

        m_data[m_size] = val;
        m_size++;
    }

    ValueType& operator[](size_t i) {
        return m_data[i];
    }

    const ValueType& operator[](size_t i) const {
        return m_data[i];
    }

    ValueType& front() {
        return m_data[0];
    }

    const ValueType& front() const {
        return m_data[0];
    }

    ValueType& back() {
        return m_data[m_size-1];
    }

    const ValueType& back() const {
        return m_data[m_size-1];
    }

    void popBack() {
        resize(m_size - 1);
    }

    bool operator==(const Vector& vec) const {
        if (m_size != vec.m_size) {
            return false;
        }

        for(size_t i = 0; i < m_size; i++) {
            if ((*this)[i] != vec[i]) {
                return false;
            }
        }

        return true;
    }


    bool operator!=(const Vector& vec) const {
        if (m_size != vec.m_size) {
            return true;
        }

        for(size_t i = 0; i < m_size; i++) {
            if ((*this)[i] != vec[i]) {
                return true;
            }
        }

        return false;
    }

    void reserve(size_t capacity) {
        if (capacity == m_capacity || capacity < m_size) {
            return;
        }

        ValueType *data = new ValueType[capacity];
        for(size_t i = 0; i < m_size; i++) {
            data[i] = m_data[i];
        }

        delete[] m_data;
        m_data = data;
        m_capacity = capacity;
    }

    void shrink() {
        return reserve(m_size);
    }

    void resize(size_t size) {
        if (size == m_size) {
            return;
        }

        if (size > m_capacity) {
            reserve(size);
        } else {
            if (size < m_size) {
                // clear data
                for(size_t s = size; s < m_size; s++) {
                    m_data[s] = ValueType();
                }
            }
        }

        m_size = size;
        return;
    }

    void clear() {
        return resize(0);
    }

    size_t capacity() const {
        return m_capacity;
    }

    size_t size() const {
        return m_size;
    }

    bool isEmpty() const {
        return size() == 0;
    }

protected:
    ValueType *m_data;
    size_t m_size;
    size_t m_capacity;
};

} // Utils

#endif // VECTOR_H
