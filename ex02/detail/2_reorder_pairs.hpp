#pragma once

#include <deque>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/detail/utils.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void reorderPairs(Container<typename ValIdx<T>::ptype,
    typename ValIdx<T>::palloc> &pairs,
    const Container<typename ValIdx<T>::type,
    typename ValIdx<T>::alloc> &bigger,
    Compare<T> cmp) {
    (void)cmp;
    Container<typename ValIdx<T>::ptype,
        typename ValIdx<T>::palloc> orderedPairs;
    for (typename Container<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc>::const_iterator it = bigger.begin();
        it != bigger.end(); ++it) {
        typename Container<typename ValIdx<T>::ptype,
            typename ValIdx<T>::palloc>
            ::const_iterator pair_it = pairs.begin();
        std::advance(pair_it, it->second);
        orderedPairs.push_back(*pair_it);
    }
    pairs.swap(orderedPairs);
}

// vector
template<typename T, template<typename> class Compare>
void reorderPairs(std::vector<typename ValIdx<T>::ptype> &pairs,
        const std::vector<typename ValIdx<T>::type> &bigger, Compare<T> cmp) {
    (void)cmp;
    std::vector<typename ValIdx<T>::ptype> orderedPairs;
    orderedPairs.reserve(pairs.size());
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs.push_back(pairs[bigger[i].second]);
    }
    pairs.swap(orderedPairs);
}

// deque
template<typename T, template<typename> class Compare>
void reorderPairs(std::deque<typename ValIdx<T>::ptype> &pairs,
        const std::deque<typename ValIdx<T>::type> &bigger, Compare<T> cmp) {
    (void)cmp;
    std::deque<typename ValIdx<T>::ptype> orderedPairs(pairs.size());
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs[i] = pairs[bigger[i].second];
    }
    pairs.swap(orderedPairs);
}

// ImplicitTreap
template<typename T, template<typename> class Compare>
void reorderPairs(ImplicitTreap<typename ValIdx<T>::ptype> &pairs,
        const ImplicitTreap<typename ValIdx<T>::type> &bigger, Compare<T> cmp) {
    (void)cmp;
    ImplicitTreap<typename ValIdx<T>::ptype> orderedPairs;
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs.insert(i, pairs[bigger[i].second]);
    }
    pairs.swap(orderedPairs);
}



