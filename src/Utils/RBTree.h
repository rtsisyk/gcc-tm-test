#ifndef RBTREE_H
#define RBTREE_H

namespace Utils {
namespace Private {

/**
 * Red-Black tree (used by TreeSet and TreeMap)
 */
template<class KeyTypeParam>
class RBTree {
public:
    typedef KeyTypeParam KeyType;

    enum Color { COLOR_BLACK, COLOR_RED };

    struct Node {
        Node *parent;
        Node *left;
        Node *right;

        KeyType key;
        Color color;
    };

    RBTree() {
        m_size = 0;
        m_root = NULL;

        m_nullNode = new Node;
        m_nullNode->key = KeyType();
        m_nullNode->left = 0;
        m_nullNode->right = 0;
        m_nullNode->parent = 0;
        m_nullNode->color = COLOR_BLACK;
    }

    // disable evil constructors
    RBTree(const RBTree& tree);
    RBTree& operator=(const RBTree& tree);

    int size() const {
        // HACK: don't use m_size for TM performance reasons
        // return m_size;

        size_t res = 0;
        for(Node *cur = minimum();
            cur != NULL && cur != m_nullNode;
            cur = successor(cur)) {
            res++;
        }

        return res;
    }

    Node *rootNode() const {
        return m_root;
    }

    Node *nullNode() const {
        return m_nullNode;
    }

    Node *find(const KeyType& key) const
    {
        Node *current = m_root;
        while(current != NULL && current != m_nullNode) {
            if(key < current->key) {
                current = current->left;
            } else if (current-> key < key){
                current = current->right;
            } else {
                while(current->left != m_nullNode && current->left->key == key) {
                    current = current->left;
                }
                return current;
            }
        }

        return NULL;
    }

    Node *minimum(Node *current = NULL) const
    {
        if(current == NULL) {
            current = m_root;
        }

        if(current == NULL || current == m_nullNode) {
            return NULL;
        }

        while(m_nullNode != current->left) {
            current = current->left;
        }

        return current;
    }

    Node *maximum(Node *current = NULL) const
    {
        if(current == NULL) {
            current = m_root;
        }

        if(current == NULL || current == m_nullNode) {
            return NULL;
        }

        while(m_nullNode != current->right) {
            current = current->right;
        }

        return current;
    }

    Node *predecessor(Node *node) const
    {
        if(node == NULL || node == m_nullNode) {
            return NULL;
        }

        if(node->left!= m_nullNode) {
            return maximum(node->left);
        } else {
            Node *current = node->parent;
            while(current != NULL && node == current->left) {
                node = current;
                current = current->parent;
            }

            return current;
        }
    }

    Node *successor(Node *node) const
    {
        if(node == NULL || node == m_nullNode) {
            return NULL;
        }

        if(node->right != m_nullNode) {
            return minimum(node->right);
        } else {
            Node *current = node->parent;
            while(current != NULL && node == current->right) {
                node = current;
                current = current->parent;
            }

            return current;
        }
    }


    Node *insert(const KeyType& key)
    {
        Node *current = m_root;
        Node *prev = NULL;

        while(current != NULL && current != m_nullNode) {
            prev = current;

            if(current->key == key) {
                // update key
                current->key = key;
                return current;
            }

            if(key < current->key ) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        return insert(key, prev);
    }

    Node *insertMulti(const KeyType& key)
    {
        Node *current = m_root;
        Node *prev = NULL;

        while(current != NULL && current != m_nullNode) {
            prev = current;

            if(key < current->key) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        return insert(key, prev);
    }

    Node *remove(Node *removeNode)
    {
        if(removeNode == NULL || removeNode == m_nullNode) {
            return NULL;
        }

        Node *successorNode = successor(removeNode);

        if(removeNode->left != m_nullNode && removeNode->right != m_nullNode) {
            /// instead of using hack below we swaps nodes to keep iterators
            //
            //removeNode->key = successorNode->key;
            //Node *swpNode = removeNode;
            //removeNode = successorNode;
            //successorNode = swpNode;

            if (successorNode->parent == removeNode) {
                removeNode->right = successorNode->right;
                successorNode->right = removeNode;

                successorNode->left = removeNode->left;
                removeNode->left = m_nullNode;
                successorNode->left->parent = successorNode;

                if (removeNode != m_root) {
                    if (removeNode->parent->left == removeNode) {
                        removeNode->parent->left = successorNode;
                    } else {
                        removeNode->parent->right = successorNode;
                    }
                } else {
                    m_root = successorNode;
                }

                successorNode->parent = removeNode->parent;
                removeNode->parent = successorNode;
            } else {
                int a = 10;
                a++;

                successorNode->left = removeNode->left;
                successorNode->left->parent = successorNode;
                removeNode->left = m_nullNode;

                Node *tmpNode = successorNode->right;
                successorNode->right = removeNode->right;
                successorNode->right->parent = successorNode;

                removeNode->right = tmpNode;
                removeNode->right->parent = successorNode;

                tmpNode = successorNode->parent;
                if (successorNode->parent->left == successorNode) {
                    successorNode->parent->left = removeNode;
                } else {
                    successorNode->parent->right = removeNode;
                }

                if (removeNode != m_root) {
                    if (removeNode->parent->left == removeNode) {
                        removeNode->parent->left = successorNode;
                    } else {
                        removeNode->parent->right = successorNode;
                    }
                } else {
                    m_root = successorNode;
                }

                successorNode->parent = removeNode->parent;
                removeNode->parent = tmpNode;
            }

            const Color swpColor = successorNode->color;
            successorNode->color = removeNode->color;
            removeNode->color = swpColor;
            removeNode->key = successorNode->key;
        }

        Node *child = m_nullNode;
        if(removeNode->left == m_nullNode) {
            child = removeNode->right;
        } else {
            child = removeNode->left;
        }

        child->parent = removeNode->parent;

        if(removeNode->parent != NULL) {
            if(removeNode == removeNode->parent->left) {
                removeNode->parent->left = child;
            } else {
                removeNode->parent->right = child;
            }
        } else {
            if(child == m_nullNode) {
                m_root = NULL;
            } else {
                m_root = child;
            }
        }

        if(removeNode->color == COLOR_BLACK && m_root != 0) {
            removeFix(child);
        }

        delete removeNode;

        // HACK: don't use m_size for TM performance reasons
        // m_size--;

        return successorNode;
    }

    struct TreeTraverserImpl {
        TreeTraverserImpl(int countMax) {
            nodes = new Node*[countMax];
            this->count = 0;
        }

        ~TreeTraverserImpl() {
            delete[] nodes;
        }

        void operator()(Node *node) {
            nodes[this->count++] = node;
        }

        Node** nodes;
        int count;
    };

    void clear() {
        size_t mysize = size();
        Node** stack = new Node*[mysize];

        size_t pos = 0;
        for(Node *cur = minimum();
            cur != NULL && cur != m_nullNode;
            cur = successor(cur)) {
            stack[pos++] = cur;
        }

        for(size_t i = 0; i < mysize; i++) {
            delete stack[i];
        }

        m_root = NULL;
    }

protected:
    enum RotateDirection { ROTATE_LEFT,  ROTATE_RIGHT };

    Node *insert(const KeyType& key, Node *prev)
    {
        Node *node = new Node;
        node->key = key;
        node->left = m_nullNode;
        node->right = m_nullNode;
        node->parent = prev;
        node->color = COLOR_RED;

        if(prev == NULL) {
            m_root = node;
        } else {
            if( node->key < prev->key ) {
                prev->left = node;
            } else {
                prev->right = node;
            }
        }

        // FIXME: hack
        // m_size++;

        insertFix(node);
        return node;
    }

    void insertFix(Node *current)
    {
        while(current != m_root && current->parent->color == COLOR_RED) {
            if(current->parent == current->parent->parent->left) {
                Node *uncle = current->parent->parent->right;
                if (uncle->color == COLOR_RED) {
                    // Case 1
                    current->parent->color = COLOR_BLACK;
                    uncle->color = COLOR_BLACK;
                    current->parent->parent->color = COLOR_RED;
                    current = current->parent->parent;
                } else {
                    if (current == current->parent->right) {
                        // Case 2
                        current = current->parent;
                        rotate(current, ROTATE_LEFT);
                    }

                    // Case 3
                    current->parent->color = COLOR_BLACK;
                    current->parent->parent->color = COLOR_RED;
                    rotate(current->parent->parent, ROTATE_RIGHT);
                }
            } else {
                Node *uncle = current->parent->parent->left;
                if(uncle->color == COLOR_RED) {
                    // Case 4
                    current->parent->color = COLOR_BLACK;
                    uncle->color = COLOR_BLACK;
                    current->parent->parent->color = COLOR_RED;
                    current = current->parent->parent;
                } else {
                    if(current == current->parent->left) {
                        // Case 5
                        current = current->parent;
                        rotate(current, ROTATE_RIGHT);
                    }
                    // Case 6
                    current->parent->color = COLOR_BLACK;
                    current->parent->parent->color = COLOR_RED;
                    rotate(current->parent->parent, ROTATE_LEFT);
                }
            }
        }

        // Keeping black root
        if (m_root->color != COLOR_BLACK) {
            m_root->color = COLOR_BLACK;
        }
    }

    void removeFix(Node *node)
    {
        while (node != m_root && node->color == COLOR_BLACK) {
            if (node == node->parent->left) {
                Node *brother = node->parent->right;
                if (brother->color == COLOR_RED) {
                    // Case 1
                    brother->color = COLOR_BLACK;
                    node->parent->color = COLOR_RED;
                    rotate(node->parent, ROTATE_LEFT);
                    brother = node->parent->right;
                }

                if (brother->left->color == COLOR_BLACK &&
                        brother->right->color == COLOR_BLACK) {
                    // Case 2
                    brother->color = COLOR_RED;
                    node = node->parent;
                } else {
                    if(brother->right->color == COLOR_BLACK)
                    {
                        // Case 3
                        brother->left->color = COLOR_BLACK;
                        brother->color = COLOR_RED;
                        rotate(brother, ROTATE_RIGHT);
                        brother = node->parent->right;
                    }

                    // Case 4
                    brother->color = node->parent->color;
                    node->parent->color = COLOR_BLACK;
                    brother->right->color = COLOR_BLACK;
                    rotate(node->parent, ROTATE_LEFT);
                    node = m_root;
                }
            } else {
                Node *brother = node->parent->left;
                if(brother->color == COLOR_RED)
                {
                    // Case 5
                    brother->color = COLOR_BLACK;
                    node->parent->color = COLOR_RED;
                    rotate(node->parent, ROTATE_RIGHT);
                    brother = node->parent->left;
                }

                if(brother->right->color == COLOR_BLACK &&
                        brother->left->color == COLOR_BLACK) {
                    // Case 6
                    brother->color = COLOR_RED;
                    node = node->parent;
                } else {
                    if(brother->left->color == COLOR_BLACK) {
                        // Case 7
                        brother->right->color = COLOR_BLACK;
                        brother->color = COLOR_RED;
                        rotate(brother, ROTATE_LEFT);
                        brother = node->parent->left;
                    }

                    // Case 8
                    brother->color = node->parent->color;
                    node->parent->color = COLOR_BLACK;
                    brother->left->color = COLOR_BLACK;
                    rotate(node->parent, ROTATE_RIGHT);
                    node = m_root;
                }
            }
        }

        if (node->color != COLOR_BLACK) {
            node->color = COLOR_BLACK;
        }
    }

    void rotate(Node *current, RotateDirection direction)
    {
        Node *child = NULL;
        if(direction == ROTATE_LEFT) {
            child = current->right;
        } else {
            child = current->left;
        }

        if(current->parent == NULL) {
            m_root = child;
        } else {
            if(current == current->parent->left) {
                current->parent->left = child;
            } else {
                current->parent->right = child;
            }
        }
        child->parent = current->parent;
        current->parent = child;

        if(direction == ROTATE_LEFT) {
            current->right = child->left;
            if(m_nullNode != current->right)
                current->right->parent = current;
            child->left = current;
        } else {
            current->left = child->right;
            if(m_nullNode != current->left)
                current->left->parent = current;
            child->right = current;

        }
    }

    int m_size;
    Node *m_root;
    Node *m_nullNode;
};

} // namespace Private
} // namespace Utils

#endif // RBTREE_H
