#pragma once

#include <algorithm>
#include <utility>
#include <vector>

template<typename Compare>
void ternarySplitQuickSortIndex(std::vector<int> &container,
    int left, int right, Compare cmp) {
    if (left >= right) {
        return;
    }
    int pivot = container[left + (right - left) / 2];
    int lt = left;
    int gt = right;
    int i = left;
    while (i <= gt) {
        if (cmp(container[i], pivot)) {
            std::swap(container[lt], container[i]);
            ++lt;
            ++i;
        } else if (container[i] == pivot) {
            ++i;
        } else {
            std::swap(container[i], container[gt]);
            --gt;
        }
    }
    ternarySplitQuickSortIndex(container, left, lt - 1, cmp);
    ternarySplitQuickSortIndex(container, gt + 1, right, cmp);
}

template<typename Compare>
void ternarySplitQuickSort(std::vector<int> &container, Compare cmp) {
    ternarySplitQuickSortIndex(container, 0, container.size() - 1, cmp);
}
