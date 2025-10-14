#pragma once

#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/compare/CLess.hpp>
#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/detail/utils.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void cmpNeighbor(const Container<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc> &container,
        Container<typename ValIdx<T>::ptype,
        typename ValIdx<T>::palloc> &pairs,
        Container<typename ValIdx<T>::type, typename ValIdx<T>::alloc> &bigger,
        Compare<T> cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;
    typename Container<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc>::const_iterator it = container.begin();
    for (std::size_t i = 0; i < bigger_size; ++i) {
        typename ValIdx<T>::type first = *it;
        ++it;
        typename ValIdx<T>::type second = *it;
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

// vector
template<typename T, template<typename> class Compare>
void cmpNeighbor(const std::vector<typename ValIdx<T>::type> &container,
        std::vector<typename ValIdx<T>::ptype> &pairs,
        std::vector<typename ValIdx<T>::type> &bigger,
        Compare<T> cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    pairs.reserve(bigger_size);
    bigger.reserve(bigger_size);

    for (std::size_t i = 0; i < bigger_size; ++i) {
        typename ValIdx<T>::type first = container[2 * i];
        typename ValIdx<T>::type second = container[2 * i + 1];
        if (cmp(first.first, second.first)) {
            pairs.push_back(std::make_pair(second, first));
            bigger.push_back(std::make_pair(second.first, i));
        } else {
            pairs.push_back(std::make_pair(first, second));
            bigger.push_back(std::make_pair(first.first, i));
        }
    }
}

// deque
template<typename T, template<typename> class Compare>
void cmpNeighbor(const std::deque<typename ValIdx<T>::type> &container,
        std::deque<typename ValIdx<T>::ptype> &pairs,
        std::deque<typename ValIdx<T>::type> &bigger,
        Compare<T> cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    pairs.resize(bigger_size);
    bigger.resize(bigger_size);
    for (std::size_t i = 0; i < bigger_size; ++i) {
        typename ValIdx<T>::type first = container[2 * i];
        typename ValIdx<T>::type second = container[2 * i + 1];
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
template<typename T, template<typename> class Compare>
void cmpNeighbor(const ImplicitTreap<typename ValIdx<T>::type> &container,
        ImplicitTreap<typename ValIdx<T>::ptype> &pairs,
        ImplicitTreap<typename ValIdx<T>::type> &bigger,
        Compare<T> cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    for (std::size_t i = 0; i < bigger_size; ++i) {
        typename ValIdx<T>::type first = container[2 * i];
        typename ValIdx<T>::type second = container[2 * i + 1];
        if (cmp(first.first, second.first)) {
            pairs.insert(i, std::make_pair(second, first));
            bigger.insert(i, std::make_pair(second.first, i));
        } else {
            pairs.insert(i, std::make_pair(first, second));
            bigger.insert(i, std::make_pair(first.first, i));
        }
    }
}
