#pragma once

#include <algorithm>
#include <utility>
#include <vector>

template<typename Compare>
void combSort(std::vector<int> &container, Compare cmp) {
    std::size_t gap = container.size();
    bool swapped = false;

    while (gap > 1 || swapped) {
        if (gap > 1) {
            gap = gap * 10 / 13;
        }
        swapped = false;
        for (std::size_t i = 0; i + gap < container.size(); ++i) {
            if (cmp(container[i + gap], container[i])) {
                std::swap(container[i], container[i + gap]);
                swapped = true;
            }
        }
    }
}
