#ifndef LINKEDLIST_H
#define LINKEDLIST_H

namespace Utils {

template<class ValueTypeParam>
class LinkedList {
public:
    typedef ValueTypeParam ValueType;
    class Iterator;

    LinkedList() {
        m_head = new Node();
        m_head->next = NULL;
        m_head->prev = NULL;
        m_size = 0;
    }

    ~LinkedList() {
        clear();
        delete m_head;
        m_head = NULL;
    }

    // disable evil constructors
    LinkedList(const LinkedList& list);
    LinkedList& operator=(const LinkedList& list);

    Iterator begin() const {
        return Iterator(this, m_head->next);
    }

    Iterator end() const {
        return Iterator(this, NULL);
    }

    Iterator pushFront(const ValueType& value) {
        return insert(m_head, value);
    }

    Iterator pushBack(const ValueType& value) {
        Node *cur = m_head;
        while (cur->next != NULL) {
            cur = cur->next;
        }

        return insert(cur, value);
    }

    Iterator insert(const Iterator& it, const ValueType& value) {
        if (it.m_node == NULL) {
            return pushBack(value);
        } else {
            return insert(it.m_node, value);
        }
    }

    Iterator remove(const Iterator& it) {
        if (it.isNull()) {
            return it;
        }

        return remove(it.m_node);
    }

    void clear() {
        Node *cur = m_head->next;
        while (cur != NULL) {
            Node *next = cur->next;
            delete cur;
            cur = next;
        }

        m_head->next = NULL;

        m_size = 0;
    }

    size_t size() const {
        return m_size;
    }

    bool isEmpty() const {
        return m_size == 0;
    }

protected:
    class Node;
public:
    /**
     * LinkedList iterator
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

        Iterator operator--(int) {
            Iterator result(m_container, m_node);
            --(*this);
            return result;
        }

        Iterator& operator++() {
            m_node = m_node->next;
            return *this;
        }

        Iterator& operator--() {
            if (m_node->prev != m_container->m_head) {
                m_node = m_node->prev;
            } else {
                m_node = NULL;
            }

            return *this;
        }

        Iterator& operator=(const Iterator& it) {
            m_container = it.m_container;
            m_node = it.m_node;
            return *this;
        }

        bool isNull() const {
            return (m_node == NULL);
        }

        bool isNotNull() const {
            return (m_node != NULL);
        }

        operator bool() const {
            return isNotNull();
        }

        bool hasPrev() const {
            return (m_node->prev != NULL && m_node->prev != m_container->m_head);
        }

        bool hasNext() const {
            return (m_node->next != NULL);
        }

        bool operator==(const Iterator& it) {
            return (m_container == it.m_container) && (m_node == it.m_node);
        }

        bool operator!=(const Iterator& it) {
            return !operator==(it);
        }

        const ValueType& value() const {
            return m_node->value;
        }

        void setValue(const ValueType& value) {
            m_node->value = value;
        }

    protected:
        friend class LinkedList;

        Iterator(const LinkedList *container, Node *node) {
            m_container = container;
            m_node = node;
        }

        const LinkedList *m_container;
        Node *m_node;
    };

protected:
    Iterator insert(Node *prev, const ValueType& value) {
        Node *node = new Node;
        node->value = value;
        node->next = prev->next;
        node->prev = prev;
        if (prev->next != NULL) {
            prev->next->prev = node;
        }
        prev->next = node;
        m_size++;
        return Iterator(this, node);
    }

    Iterator remove(Node *cur) {
        printf("Remove: %d\n", cur->value);
        if (cur->prev != NULL) {
            cur->prev->next = cur->next;
        }

        if (cur->next != NULL) {
            cur->next->prev = cur->prev;
        }

        Node *next = cur->next;
        delete cur;

        m_size--;
        return Iterator(this, next);
    }

    struct Node {
        Node *prev;
        Node *next;
        ValueType value;
    };

    Node *m_head;
    size_t m_size;
};

}
#endif // LINKEDLIST_H
