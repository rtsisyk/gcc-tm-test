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

#ifndef HASHMAP_H
#define HASHMAP_H

#include <limits.h>
#include <algorithm>

namespace Utils {

/**
 * Map implementation based on hash table
 */
template<class KeyTypeParam, class ValueTypeParam>
class HashMap {
public:
    typedef KeyTypeParam KeyType;
    typedef ValueTypeParam ValueType;

    class Iterator;

    HashMap(size_t bucketsCount = 1024) {
        const size_t MIN_BUCKETS = 1024;
        const size_t MAX_BUCKETS = (size_t) 1 << (sizeof(size_t) * CHAR_BIT - 1);

        if (bucketsCount < MIN_BUCKETS) {
            m_bucketsCount = MIN_BUCKETS;
        } else if (bucketsCount > MAX_BUCKETS) {
            m_bucketsCount = MAX_BUCKETS;
        } else {
            m_bucketsCount = pow2roundup(bucketsCount);
        }

        m_mask = m_bucketsCount-1;
        m_buckets = new Node[m_bucketsCount];

        for(size_t b = 0; b < m_bucketsCount; b++) {
            m_buckets[b].next = NULL;
        }
    }

    ~HashMap() {
        clear();
        delete [] m_buckets;
    }

    // disable evil constructors
    HashMap(const HashMap& map);
    HashMap& operator=(const HashMap& map);

    Iterator begin() const {
        return Iterator(this, firstNode());
    }

    Iterator end() const {
        return Iterator(this, NULL);
    }

    Iterator find(const KeyType& key) const {
        size_t h = hash(key) & m_mask;
        Node *cur = m_buckets[h].next;

        while (cur != NULL && cur->key <= key) {
            if (cur->key == key) {
                return Iterator(this, cur);
            }

            cur = cur->next;
        }

        return Iterator(this, NULL);
    }

    Iterator find(const KeyType& key, const ValueType& value) const {
        for(Iterator it = find(key); it != end() && it.key() == key; it++) {
            if (it.value() == value) {
                return it;
            }
        }

        return end();
    }

    bool contains(const KeyType& key) const {
        return (find(key) != end());
    }

    bool contains(const KeyType& key, const ValueType& value) const {
        return (find(key, value) != end());
    }

    size_t count(const KeyType& key) const {
        size_t result = 0;
        for(Iterator it = find(key); it != end() && it.key() == key; it++) {
            result++;
        }

        return result;
    }

    size_t count(const KeyType& key, const ValueType& value) const {
        size_t result = 0;
        for(Iterator it = find(key); it != end() && it.key() == key; it++) {
            if (it.value() == value) {
                result++;
            }
        }

        return result;
    }

    Iterator insert(const KeyType& key, const ValueType& value) {
        size_t h = hash(key) & m_mask;
        Node *cur = &(m_buckets[h]);

        while (cur->next != NULL && cur->next->key <= key) {
            cur = cur->next;
        }

        if (cur != &(m_buckets[h]) && cur->key == key) {
            // update key;
            cur->value = value;
            return Iterator(this, cur);
        }

        return insert(key, value, cur);
    }

    Iterator insertMulti(const KeyType& key, const ValueType& value) {
        size_t h = hash(key) & m_mask;
        Node *cur = &(m_buckets[h]);
        while (cur->next != NULL && cur->next->key <= key) {
            cur = cur->next;
        }

        return insert(key, value, cur);
    }

    Iterator remove(const Iterator& it) {
        if (it.isNull() || it.m_container != this) {
            return end();
        } else {
            Node *need = it.m_node;
            size_t h = hash(need->key) & m_mask;
            Node *cur = &(m_buckets[h]);
            while (cur->next != NULL && cur->next->key <= need->key && cur->next != need) {
                cur = cur->next;
            }

            if (cur->next != need) {
                return end();
            } else {
                cur->next = cur->next->next;
                delete need;
                return ++Iterator(this, cur);
            }
        }
    }

    Iterator removeAll(const KeyType& key) {
        Iterator it = find(key);
        while (it != end() && it.key() == key) {
            it = remove(it);
        }

        return it;
    }

    void clear() {
        for(size_t h = 0; h < m_bucketsCount; h++) {
            Node *cur = m_buckets[h].next;
            while (cur != NULL) {
                Node *rem = cur;
                cur = cur->next;
                delete rem;
            }

            m_buckets[h].next = NULL;
        }
    }

    void reserve(size_t bucketsCount) {
        if (m_bucketsCount >= bucketsCount) {
            return;
        }

        HashMap map(bucketsCount);
        for(Iterator it = begin(); it != end(); it++) {
            map.insertMulti(it.key(), it.value());
        }

        size_t oBucketsCount = m_bucketsCount;
        size_t oldMask = m_mask;
        Node *oldBuckets = m_buckets;

        m_bucketsCount = map.m_bucketsCount;
        m_mask = map.m_mask;
        m_buckets = map.m_buckets;

        map.m_bucketsCount = oBucketsCount;
        map.m_mask = oldMask;
        map.m_buckets = oldBuckets;
    }

    bool isEmpty() const {
        return  begin() == end();
    }

protected:
    struct Node {
        KeyType key;
        ValueType value;
        Node *next;
    };

public:
    /**
     * Set iterator
     */
    class Iterator {
    public:
        Iterator(const Iterator& it) {
            m_container = it.m_container;
            m_node = it.m_node;
        }

        Iterator operator++(int) {
            Iterator result(m_container, m_node);
            ++(*this);
            return result;
        }

        Iterator& operator++() {
            m_node = m_container->nextNode(m_node);
            return *this;
        }

        Iterator& operator=(const Iterator& it) {
            m_container = it.m_container;
            m_node = it.m_node;
            return *this;
        }

        bool operator==(const Iterator& it) {
            return (m_container == it.m_container) && (m_node == it.m_node);
        }

        bool operator!=(const Iterator& it) {
            return !operator==(it);
        }

        const KeyType& key() const {
            return m_node->key;
        }

        const KeyType& operator*() const {
            return key();
        }

        const ValueType& value() const {
            return m_node->value;
        }

        void setValue(const ValueType& value) {
            m_node->value = value;
        }

        bool isNull() const {
            return (m_node == NULL);
        }

        bool isNotNull() const {
            return (m_node != NULL);
        }

    protected:
        friend class HashMap;

        Iterator(const HashMap *container, Node *node) {
            m_container = container;
            m_node = node;
        }

        const HashMap *m_container;
        Node *m_node;
    };


protected:
    inline size_t pow2roundup(size_t x) const {
        x--;

        for (size_t p = 1; p < sizeof(size_t) * CHAR_BIT; p <<=1) {
            x |= x >> p;
        }

        return x+1;
    }

    size_t hash(const KeyType& key) const {
        return hasher(key);
    }

    Iterator insert(const KeyType& key, const ValueType& value, Node *prev) {
        Node *insertNode = new Node();
        insertNode->key = key;
        insertNode->value = value;
        insertNode->next = prev->next;
        prev->next = insertNode;

        return Iterator(this, insertNode);
    }

    Node *firstNode() const {
        for(size_t h = 0; h < m_bucketsCount; h++) {
            if (m_buckets[h].next != NULL) {
                return m_buckets[h].next;
            }
        }

        return NULL;
    }

    Node *nextNode(Node *cur) const {
        if (cur == NULL) {
            return NULL;
        }

        if (cur->next != NULL) {
            return cur->next;
        }

        size_t h = hash(cur->key) & m_mask;
        h++;
        for(; h < m_bucketsCount; h++) {
            if (m_buckets[h].next != NULL) {
                return m_buckets[h].next;
            }
        }

        return NULL;
    }

    size_t m_bucketsCount;
    size_t m_mask;
    Node *m_buckets;

    std::hash<KeyType> hasher;
};

} // namespace Utils

#endif // HASHMAP_H
