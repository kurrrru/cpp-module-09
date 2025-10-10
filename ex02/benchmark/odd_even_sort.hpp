#pragma once

#include <algorithm>
#include <vector>

template<typename Compare>
void oddEvenSort(std::vector<int> &container, Compare cmp) {
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        for (std::size_t i = 1; i < container.size() - 1; i += 2) {
            if (cmp(container[i + 1], container[i])) {
                std::swap(container[i], container[i + 1]);
                sorted = false;
            }
        }
        for (std::size_t i = 0; i < container.size() - 1; i += 2) {
            if (cmp(container[i + 1], container[i])) {
                std::swap(container[i], container[i + 1]);
                sorted = false;
            }
        }
    }
}
