#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

template<typename Compare>
void binaryInsertionSort(std::vector<int> &container, Compare cmp) {
    for (std::size_t i = 1; i < container.size(); ++i) {
        int key = container[i];
        std::vector<int>::iterator pos = std::lower_bound(container.begin(), container.begin() + i, key, cmp);
        std::rotate(pos, container.begin() + i, container.begin() + i + 1);
    }
}