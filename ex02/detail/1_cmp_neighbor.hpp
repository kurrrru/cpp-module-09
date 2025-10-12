#pragma once

#include <memory>
#include <utility>

#include <ex02/datastructure/ImplicitTreap.hpp>

template<typename T, template<typename, typename> class Container, typename Compare>
void cmpNeighbor(const Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &container,
        Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &bigger,
        Compare cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;
    typename Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > >::const_iterator it = container.begin();
    for (std::size_t i = 0; i < bigger_size; ++i) {
        std::pair<T, std::size_t> first = *it;
        ++it;
        std::pair<T, std::size_t> second = *it;
        ++it;
        if (cmp(first.first, second.first)) {
            pairs.push_back(std::make_pair(second, first));
            bigger.push_back(std::make_pair(second.first, i));
        } else {
            pairs.push_back(std::make_pair(first, second));
            bigger.push_back(std::make_pair(first.first, i));
        }
    }
}

// // vector
template<typename T, typename Compare>
void cmpNeighbor(const std::vector<std::pair<T, std::size_t> > &container,
        std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::vector<std::pair<T, std::size_t> > &bigger,
        Compare cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    pairs.reserve(bigger_size);
    bigger.reserve(bigger_size);

    for (std::size_t i = 0; i < bigger_size; ++i) {
        std::pair<T, std::size_t> first = container[2 * i];
        std::pair<T, std::size_t> second = container[2 * i + 1];
        if (cmp(first.first, second.first)) {
            pairs.push_back(std::make_pair(second, first));
            bigger.push_back(std::make_pair(second.first, i));
        } else {
            pairs.push_back(std::make_pair(first, second));
            bigger.push_back(std::make_pair(first.first, i));
        }
    }
}

// // deque
template<typename T, typename Compare>
void cmpNeighbor(const std::deque<std::pair<T, std::size_t> > &container,
        std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::deque<std::pair<T, std::size_t> > &bigger,
        Compare cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    pairs.resize(bigger_size);
    bigger.resize(bigger_size);
    for (std::size_t i = 0; i < bigger_size; ++i) {
        std::pair<T, std::size_t> first = container[2 * i];
        std::pair<T, std::size_t> second = container[2 * i + 1];
        if (cmp(first.first, second.first)) {
            pairs[i] = std::make_pair(second, first);
            bigger[i] = std::make_pair(second.first, i);
        } else {
            pairs[i] = std::make_pair(first, second);
            bigger[i] = std::make_pair(first.first, i);
        }
    }
}

// ImplicitTreap
template<typename T, typename Compare>
void cmpNeighbor(const ImplicitTreap<T> &container,
        ImplicitTreap<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        ImplicitTreap<std::pair<T, std::size_t> > &bigger,
        Compare cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    for (std::size_t i = 0; i < bigger_size; ++i) {
        std::pair<T, std::size_t> first = container[2 * i];
        std::pair<T, std::size_t> second = container[2 * i + 1];
        if (cmp(first.first, second.first)) {
            pairs.insert(i, std::make_pair(second, first));
            bigger.insert(i, std::make_pair(second.first, i));
        } else {
            pairs.insert(i, std::make_pair(first, second));
            bigger.insert(i, std::make_pair(first.first, i));
        }
    }
}
