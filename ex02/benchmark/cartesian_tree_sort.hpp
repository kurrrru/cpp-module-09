#pragma once

#include <vector>

#include <ex02/datastructure/Cartesian.hpp>

template<typename Compare>
void cartesianTreeSort(std::vector<int> &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    (void)cmp;
    Cartesian<Compare> tree(container);
    container = tree.sort();
}
