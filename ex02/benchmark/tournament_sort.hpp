#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

// template<typename Compare>
// void heapIncreaseKey(std::vector<int> &heap, std::size_t index,
//     int newValue, Compare cmp) {
//     heap[index] = newValue;
//     while (index > 0) {
//         std::size_t parent = (index - 1) / 2;
//         if (cmp(heap[index], heap[parent])) {
//             std::swap(heap[index], heap[parent]);
//             index = parent;
//         } else {
//             break;
//         }
//     }
// }

template<typename Compare>
void heapDecreaseKey(std::vector<int> &heap, std::size_t index,
    int newValue, Compare cmp) {
    heap[index] = newValue;
    std::size_t n = heap.size();
    while (true) {
        std::size_t left = 2 * index + 1;
        std::size_t right = 2 * index + 2;
        std::size_t largest = index;
        if (left < n && cmp(heap[largest], heap[left])) {
            largest = left;
        }
        if (right < n && cmp(heap[largest], heap[right])) {
            largest = right;
        }
        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            index = largest;
        } else {
            break;
        }
    }
}

template<typename Compare>
void tournamentSort(std::vector<int> &container, Compare cmp) {
    // assuming that the heap is a max-heap
    const int MINUS_INF = std::numeric_limits<int>::min();
    std::vector<int> tournamentTree = container;
    std::make_heap(tournamentTree.begin(), tournamentTree.end(), cmp);
    for (int i = container.size() - 1; i >= 0; --i) {
        container[i] = tournamentTree.front();
        heapDecreaseKey(tournamentTree, 0, MINUS_INF, cmp);
    }
}
