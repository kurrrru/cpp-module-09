#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

template<typename Compare>
void mergeSort(std::vector<int> &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t mid = container.size() / 2;
    std::vector<int> left(container.begin(), container.begin() + mid);
    std::vector<int> right(container.begin() + mid, container.end());
    mergeSort(left, cmp);
    mergeSort(right, cmp);
    std::merge(left.begin(), left.end(), right.begin(), right.end(),
        container.begin(), cmp);
}

