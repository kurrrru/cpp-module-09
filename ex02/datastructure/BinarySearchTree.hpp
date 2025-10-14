#pragma once

#include <vector>

template<typename Compare>
class BinarySearchTree {
 public:
    BinarySearchTree(): _root(NULL) {}
    ~BinarySearchTree() {
        clear();
    }

    void insert(int value) {
        insert(_root, value);
    }

    std::vector<int> in_order_traversal() {
        std::vector<int> result;
        in_order_traversal(_root, result);
        return result;
    }

    void clear() {
        clear(_root);
    }
 private:
    struct Node {
        int _value;
        Node* _child[2];  // _child[0]: left, _child[1]: right
        Node(int val) : _value(val) {
            _child[0] = NULL;
            _child[1] = NULL;
        }
    };

    Node* _root;

    void insert(Node*& node, int value) {
        if (!node) {
            node = new Node(value);
            return;
        }
        insert(node->_child[Compare()(node->_value, value)], value);
    }

    void in_order_traversal(Node* node, std::vector<int>& result) {
        if (!node) {
            return;
        }
        in_order_traversal(node->_child[0], result);
        result.push_back(node->_value);
        in_order_traversal(node->_child[1], result);
    }

    void clear(Node* node) {
        if (!node) {
            return;
        }
        clear(node->_child[0]);
        clear(node->_child[1]);
        delete node;
        node = NULL;
    }

    BinarySearchTree(const BinarySearchTree &);
    BinarySearchTree &operator=(const BinarySearchTree &);
};
