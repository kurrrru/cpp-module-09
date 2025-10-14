#pragma once

#include <utility>
#include <vector>

template<typename Compare>
void bubbleSort(std::vector<int> &container, Compare cmp) {
    bool swapped;
    for (std::size_t i = 0; i < container.size() - 1; ++i) {
        swapped = false;
        for (std::size_t j = container.size() - 1; j > i; --j) {
            if (cmp(container[j], container[j - 1])) {
                std::swap(container[j], container[j - 1]);
                swapped = true;
            }
        }
        if (!swapped) {
            break;
        }
    }
}
