#pragma once

#include <algorithm>
#include <limits>
#include <vector>

template<typename Compare>
std::vector<int> mergePiles(std::vector<std::vector<int> > piles,
        std::size_t n, Compare cmp) {
    std::vector<int> result;
    result.reserve(n);
    while (true) {
        int minValue = std::numeric_limits<int>::max();
        int minIndex = -1;
        for (std::size_t i = 0; i < piles.size(); ++i) {
            if (!piles[i].empty() && cmp(piles[i].back(), minValue)) {
                minValue = piles[i].back();
                minIndex = i;
            }
        }
        if (minIndex == -1) {
            break;
        }
        result.push_back(minValue);
        piles[minIndex].pop_back();
        if (piles[minIndex].empty()) {
            piles.erase(piles.begin() + minIndex);
        }
    }
    return result;
}

template<typename Compare>
void patienceSort(std::vector<int> &container, Compare cmp) {
    std::size_t n = container.size();
    std::vector<std::vector<int> > piles;
    for (std::size_t i = 0; i < n; ++i) {
        if (piles.empty()) {
            std::vector<int> new_pile;
            new_pile.push_back(container[i]);
            piles.push_back(new_pile);
        } else {
            bool placed = false;
            for (std::size_t j = 0; j < piles.size(); ++j) {
                if (cmp(container[i], piles[j].back())) {
                    piles[j].push_back(container[i]);
                    placed = true;
                    break;
                }
            }
            if (!placed) {
                std::vector<int> new_pile;
                new_pile.push_back(container[i]);
                piles.push_back(new_pile);
            }
        }
    }
    container = mergePiles(piles, n, cmp);
}
