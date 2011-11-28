#ifndef TREEMAP_H
#define TREEMAP_H

#include "RBTree.h"

namespace Utils {

/**
 * Map implementation based on RBTree
 */
template<class KeyTypeParam, class ValueTypeParam>
class TreeMap {
public:
    typedef KeyTypeParam KeyType;
    typedef ValueTypeParam ValueType;

    class Iterator;

    TreeMap() {
        // nothing
    }

    ~TreeMap() {
        // nothing
    }

    // disable evil constructors
    TreeMap(const TreeMap& map);
    TreeMap& operator=(const TreeMap& map);

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
        return Iterator(this, m_tree.find(MapNode(key)));
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
        TreeNode *node = m_tree.insert(MapNode(key, value));
        return Iterator(this, node);
    }

    Iterator insertMulti(const KeyType& key, const ValueType& value) {
        return Iterator(this, m_tree.insertMulti(MapNode(key, value)));
    }

    Iterator remove(const Iterator& it) {
        if (it.m_container != this) {
            return end();
        } else {
            return Iterator(this, m_tree.remove(it.m_node));
        }
    }

    __attribute__((transaction_safe))
    Iterator removeAll(const KeyType& key) {
        TreeNode *node = NULL;

        for (node = m_tree.find(MapNode(key)); node != NULL && node->key.key() == key; ) {
            node = m_tree.remove(node);
        }

        return Iterator(this, node);
    }

    ValueType take(const KeyType& key) {
        TreeNode *node = m_tree.find(MapNode(key));
        if (node != NULL) {
            const ValueType result = node->key.value();
            m_tree.remove(node);
            return result;
        } else {
            return ValueType();
        }
    }

    void clear() {
        return m_tree.clear();
    }

    size_t size() const {
        return m_tree.size();
    }

    bool isEmpty() const {
        return size() == 0;
    }

protected:
    class MapNode {
    public:
        __attribute__((transaction_safe))
        MapNode() {
            m_key = KeyType();
            m_value = ValueType();
        }

        __attribute__((transaction_safe))
        MapNode(const KeyType& key) {
            m_key = key;
            m_value = ValueType();
        }

        __attribute__((transaction_safe))
        MapNode(const KeyType& key, const ValueType& value) {
            m_key = key;
            m_value = value;
        }

        __attribute__((transaction_safe))
        MapNode(const MapNode& node) {
            m_key = node.m_key;
            m_value = node.m_value;
        }

        bool operator<(const MapNode& node) const {
            return m_key < node.m_key;
        }

        bool operator==(const MapNode& node) const {
            return m_key == node.m_key;
        }

        const KeyType& key() const {
            return m_key;
        }

        const ValueType& value() const {
            return m_value;
        }

        void setValue(const ValueType& value) {
            m_value = value;
        }

    protected:
        KeyType m_key;
        ValueType m_value;
    };

    typedef Private::RBTree<MapNode> Tree;
    typedef typename Private::RBTree<MapNode>::Node TreeNode;

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
            return m_node->key.key();
        }

        const ValueType& value() const {
            return m_node->key.value();
        }

        void setValue(const ValueType& value) {
            m_node->key.setValue(value);
        }

    protected:
        friend class TreeMap;

        Iterator(const TreeMap *map, TreeNode *node) {
            m_container = map;
            m_node = node;

            checkNull();
        }

        void checkNull() {
            if (m_node == NULL) {
                m_node = m_container->m_tree.nullNode();
            }
        }

        const TreeMap *m_container;
        TreeNode *m_node;
    };

protected:
    Tree m_tree;
};

} // namespace Utils

#endif // TREEMAP_H
