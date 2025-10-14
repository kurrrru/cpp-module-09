#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

template<typename Compare>
void insertionSort(std::vector<int> &container, Compare cmp) {
    for (std::size_t i = 1; i < container.size(); ++i) {
        int key = container[i];
        std::size_t j = i;
        while (j > 0 && cmp(key, container[j - 1])) {
            --j;
        }
        std::rotate(container.begin() + j, container.begin() + i,
            container.begin() + i + 1);
    }
}

template<typename Iterator, typename Compare>
void insertionSortIt(Iterator begin, Iterator end, Compare cmp) {
    if (std::distance(begin, end) <= 1) {
        return;
    }
    Iterator it = begin;
    ++it;
    for (; it != end; ++it) {
        int key = *it;
        Iterator j = it;
        while (j != begin) {
            Iterator prev = j;
            --prev;
            if (cmp(key, *prev)) {
                --j;
            } else {
                break;
            }
        }
        std::rotate(j, it, it + 1);
    }
}

