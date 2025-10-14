#pragma once

#include <algorithm>
#include <utility>
#include <vector>

template<typename Compare>
void selectionSort(std::vector<int> &container, Compare cmp) {
    for (std::size_t i = 0; i < container.size() - 1; ++i) {
        std::vector<int>::iterator min_it = std::min_element(
            container.begin() + i, container.end(), cmp);
        if (min_it != container.begin() + i) {
            std::swap(container[i], *min_it);
        }
    }
}
