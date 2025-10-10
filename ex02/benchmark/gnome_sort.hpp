#pragma once

#include <algorithm>
#include <vector>

template <typename Compare>
void gnomeSort(std::vector<int> &container, Compare cmp) {
    std::size_t index = 1;
    while (index < container.size()) {
        if (cmp(container[index - 1], container[index]) || container[index - 1] == container[index]) {
            ++index;
        } else {
            std::swap(container[index - 1], container[index]);
            --index;
            if (index == 0) {
                ++index;
            }
        }
    }
}
