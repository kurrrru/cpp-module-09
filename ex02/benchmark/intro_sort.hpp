#pragma once

#include <algorithm>
#include <iterator>
#include <cmath>
#include <vector>

#include <ex02/benchmark/heap_sort.hpp>
#include <ex02/benchmark/binary_insertion_sort.hpp>

namespace {

template<typename Iterator, typename Compare>
int medianToLast(Iterator begin, Iterator mid, Iterator last, Compare cmp) {
    if (cmp(*mid, *begin)) {
        std::swap(*mid, *begin);
    }
    if (cmp(*last, *mid)) {
        std::swap(*last, *mid);
    }
    if (cmp(*mid, *begin)) {
        std::swap(*mid, *begin);
    }
    std::swap(*mid, *last);
    return *last;
}

template<typename Iterator, typename Compare>
void introSortImpl(Iterator begin, Iterator end, std::size_t depthLimit, Compare cmp) {
    std::size_t size = std::distance(begin, end);
    if (size <= 16) {
        binaryInsertionSortIt(begin, end, cmp);
        return;
    }
    if (depthLimit == 0) {
        heapSortIt(begin, end, cmp);
        return;
    }
    int pivot = medianToLast(begin, begin + size / 2, end - 1, cmp);
    Iterator pivot_pos = std::partition(begin, end - 1, IsLessThan<Compare>(pivot, cmp));
    std::swap(*pivot_pos, *(end - 1));
    introSortImpl(begin, pivot_pos, depthLimit - 1, cmp);
    introSortImpl(pivot_pos + 1, end, depthLimit - 1, cmp);
}
}

template<typename Compare>
void introSort(std::vector<int> &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t depthLimit = static_cast<std::size_t>(std::log(container.size()) / std::log(2));
    introSortImpl(container.begin(), container.end(), depthLimit, cmp);
}