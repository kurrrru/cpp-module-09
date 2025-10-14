#pragma once

#include <vector>

#include <ex02/datastructure/BinarySearchTree.hpp>

template<typename Compare>
void treeSort(std::vector<int> &container, Compare cmp) {
    (void)cmp;
    BinarySearchTree<Compare> bst;
    for (std::size_t i = 0; i < container.size(); ++i) {
        bst.insert(container[i]);
    }
    container = bst.in_order_traversal();
}
