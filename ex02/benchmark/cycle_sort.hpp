#pragma once

#include <algorithm>
#include <vector>

template<typename Compare>
void cycleSort(std::vector<int> &container, Compare cmp) {
    std::size_t n = container.size();
    for (std::size_t cycle_start = 0; cycle_start < n - 1; ++cycle_start) {
        int item = container[cycle_start];
        std::size_t pos = cycle_start;
        for (std::size_t i = cycle_start + 1; i < n; ++i) {
            if (cmp(container[i], item)) {
                ++pos;
            }
        }
        if (pos == cycle_start) {
            continue;
        }
        while (item == container[pos]) {
            ++pos;
        }
        if (pos != cycle_start) {
            std::swap(item, container[pos]);
        }
        while (pos != cycle_start) {
            pos = cycle_start;
            for (std::size_t i = cycle_start + 1; i < n; ++i) {
                if (cmp(container[i], item)) {
                    ++pos;
                }
            }
            while (item == container[pos]) {
                ++pos;
            }
            if (item != container[pos]) {
                std::swap(item, container[pos]);
            }
        }
    }
}