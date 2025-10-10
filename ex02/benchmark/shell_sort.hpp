#pragma once

#include <algorithm>
#include <vector>
#include <cmath>


std::size_t pow(std::size_t base, std::size_t exp) {
    std::size_t result = 1;
    while (exp) {
        if (exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

void generateGapSequence(std::vector<std::size_t> &gaps, std::size_t max_value) {
    gaps.clear();
    gaps.push_back(1);
    std::size_t k = 1;
    while (true) {
        std::size_t gap = pow(4, k) + 3 * pow(2, k - 1) + 1;
        if (gap > max_value) {
            break;
        }
        gaps.push_back(gap);
        ++k;
    }
}

template<typename Compare>
void shellSort(std::vector<int> &container, Compare cmp) {
    std::size_t n = container.size();
    std::vector<std::size_t> gaps;
    generateGapSequence(gaps, n / 2);
    for (int i = gaps.size() - 1; i >= 0; --i) {
        std::size_t gap = gaps[i];
        for (std::size_t j = gap; j < n; ++j) {
            int temp = container[j];
            std::size_t k = j;
            while (k >= gap && cmp(temp, container[k - gap])) {
                container[k] = container[k - gap];
                k -= gap;
            }
            container[k] = temp;
        }
    }
}
