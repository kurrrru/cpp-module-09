#pragma once

#include <algorithm>
#include <utility>
#include <vector>

template<typename Compare>
void shakerSort(std::vector<int> &container, Compare cmp) {
    std::size_t left = 0;
    std::size_t right = container.size() - 1;
    while (true) {
        std::size_t last_swap = left;
        for (std::size_t i = left; i < right; ++i) {
            if (cmp(container[i + 1], container[i])) {
                std::swap(container[i], container[i + 1]);
                last_swap = i;
            }
        }
        right = last_swap;
        if (left == right) {
            break;
        }
        last_swap = right;
        for (std::size_t i = right; i > left; --i) {
            if (cmp(container[i], container[i - 1])) {
                std::swap(container[i], container[i - 1]);
                last_swap = i;
            }
        }
        left = last_swap;
        if (left == right) {
            break;
        }
    }
}
