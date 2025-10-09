#pragma once

#include <algorithm>
#include <vector>

template<typename Compare>
void heapSort(std::vector<int> &container, Compare cmp) {
    std::make_heap(container.begin(), container.end(), cmp);
    std::sort_heap(container.begin(), container.end(), cmp);
}
