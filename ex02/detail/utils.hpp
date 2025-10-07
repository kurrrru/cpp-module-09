#pragma once

#include <utility>

void nextJacobsthal(std::pair<std::size_t, std::size_t> &j);

template <typename T, typename Compare>
struct ComparePairFirst {
    bool operator()(const std::pair<T, bool> &a, const std::pair<T, bool> &b) {
        return Compare()(a.first, b.first);
    }
};
