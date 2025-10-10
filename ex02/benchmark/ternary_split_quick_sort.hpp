#pragma once

#include <algorithm>
#include <vector>

template<typename Compare>
void inplaceTernarySplitQuickSort(std::vector<int> &container, int left, int right, Compare cmp) {
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
    inplaceTernarySplitQuickSort(container, left, lt - 1, cmp);
    inplaceTernarySplitQuickSort(container, gt + 1, right, cmp);
}

template<typename Compare>
void ternarySplitQuickSort(std::vector<int> &container, Compare cmp) {
    inplaceTernarySplitQuickSort(container, 0, container.size() - 1, cmp);
}
