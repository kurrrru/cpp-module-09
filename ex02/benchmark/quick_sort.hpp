#pragma once

#include <algorithm>
#include <utility>
#include <vector>

template<typename Compare>
struct IsLessThan {
    int pivot_value;
    Compare cmp;
    IsLessThan(int pivot, Compare c) : pivot_value(pivot), cmp(c) {}
    bool operator()(int value) const {
        return cmp(value, pivot_value);
    }
};

template<typename Iterator, typename Compare>
void quickSortIt(Iterator begin, Iterator end, Compare cmp) {
    if (std::distance(begin, end) <= 1) {
        return;
    }
    Iterator last = end;
    std::advance(last, -1);
    int pivot = *last;
    Iterator pivot_pos = std::partition(begin, last,
        IsLessThan<Compare>(pivot, cmp));
    std::swap(*pivot_pos, *last);
    quickSortIt(begin, pivot_pos, cmp);
    std::advance(pivot_pos, 1);
    quickSortIt(pivot_pos, end, cmp);
}

template<typename Compare>
void quickSort(std::vector<int> &container, Compare cmp) {
    quickSortIt(container.begin(), container.end(), cmp);
}
