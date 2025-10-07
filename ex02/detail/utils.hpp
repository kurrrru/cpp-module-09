#pragma once

#include <utility>

void nextJacobsthal(std::pair<std::size_t, std::size_t> &j);

template <typename T, typename Compare>
struct ComparePairFirstFirst {
    bool operator()(const std::pair<std::pair<T, std::size_t>, bool> &a, const std::pair<std::pair<T, std::size_t>, bool> &b) {
        return Compare()(a.first.first, b.first.first);
    }
};
