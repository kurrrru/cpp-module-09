#pragma once

#include <algorithm>
#include <vector>

template<typename Compare>
void heapSort(std::vector<int> &container, Compare cmp) {
    std::make_heap(container.begin(), container.end(), cmp);
    std::sort_heap(container.begin(), container.end(), cmp);
}

template<typename Iterator, typename Compare>
void heapSortIt(Iterator begin, Iterator end, Compare cmp) {
    std::make_heap(begin, end, cmp);
    std::sort_heap(begin, end, cmp);
}
