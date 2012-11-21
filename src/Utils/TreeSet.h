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

#ifndef TREESET_H
#define TREESET_H

#include "RBTree.h"

namespace Utils {

/**
 * Set implementation based on RBTree
 */
template<class KeyTypeParam>
class TreeSet {
public:
    typedef KeyTypeParam KeyType;
    class Iterator;

    TreeSet() {
        // nothing
    }

    ~TreeSet() {
        // nothing
    }

    // disable evil constructors
    TreeSet(const TreeSet& set);
    TreeSet& operator=(const TreeSet& set);

    Iterator begin() const {
        return minimum();
    }

    Iterator end() const {
        return Iterator(this, m_tree.nullNode());
    }

    Iterator minimum() const {
        return Iterator(this, m_tree.minimum());
    }

    Iterator maximum() const {
        return Iterator(this, m_tree.maximum());
    }

    Iterator find(const KeyType& key) const {
        return Iterator(this, m_tree.find(key));
    }

    bool contains(const KeyType& key) const {
        return (m_tree.find(key) != end());
    }

    size_t count(const KeyType& key) const {
        size_t result = 0;
        for(Iterator it = find(key); it != end() && it.key() == key; it++) {
            result++;
        }

        return result;
    }

    Iterator insert(const KeyType& key) {
        return Iterator(this, m_tree.insert(key));
    }

    Iterator insertMulti(const KeyType& key) {
        return Iterator(this, m_tree.insertMulti(key));
    }

    Iterator remove(const Iterator& it) {
        if (it.m_container != this) {
            return end();
        } else {
            return Iterator(this, m_tree.remove(it.m_node));
        }
    }

    Iterator removeAll(const KeyType& key) {
        TreeNode *node = NULL;

        for (node = m_tree.find(key); node != NULL && node->key == key; ) {
            node = m_tree.remove(node);
        }

        return Iterator(this, node);
    }

    KeyType take(const KeyType& key) {
        TreeNode *node = m_tree.find(key);
        if (node != NULL) {
            const KeyType result = node->key;
            m_tree.remove(node);
            return result;
        } else {
            return KeyType();
        }
    }

    void clear() {
        return m_tree.clear();
    }

    size_t size() const {
        return m_tree.size();
    }

    bool isEmpty() const {
        return  begin() == end();
    }

protected:
    typedef Private::RBTree<KeyTypeParam> Tree;
    typedef typename Private::RBTree<KeyTypeParam>::Node TreeNode;

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
            m_node = m_container->m_tree.successor(m_node);
            checkNull();

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

    protected:
        friend class TreeSet;

        Iterator(const TreeSet *set, TreeNode *node) {
            m_container = set;
            m_node = node;

            checkNull();
        }

        void checkNull() {
            if (m_node == NULL) {
                m_node = m_container->m_tree.nullNode();
            }
        }

        const TreeSet *m_container;
        TreeNode *m_node;
    };

protected:
    Tree m_tree;
};

} // namespace Utils

#endif // TREESET_H

