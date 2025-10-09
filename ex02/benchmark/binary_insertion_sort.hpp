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

template<typename Iterator, typename Compare>
void binaryInsertionSortIt(Iterator begin, Iterator end, Compare cmp) {
    if (std::distance(begin, end) <= 1) {
        return;
    }
    Iterator it = begin;
    ++it;
    for (; it != end; ++it) {
        int key = *it;
        Iterator pos = std::lower_bound(begin, it, key, cmp);
        std::rotate(pos, it, it + 1);
    }
}
