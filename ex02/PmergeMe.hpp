#pragma once

#include <algorithm>
#include <deque>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/PmergeMeIndexed.hpp>
#include <ex02/datastructure/ImplicitTreap.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void PmergeMeSort(Container<T, std::allocator<T> > &container,
    Compare<T> cmp) {
    Container<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc> indexedContainer;
    std::size_t index = 0;
    for (typename Container<T, std::allocator<T> >::iterator
        it = container.begin(); it != container.end(); ++it, ++index) {
        indexedContainer.push_back(std::make_pair(*it, index));
    }
    PmergeMeSortIndexed(indexedContainer, cmp);
    typename Container<T, std::allocator<T> >::iterator it = container.begin();
    typename Container<typename ValIdx<T>::type, typename ValIdx<T>::alloc>
        ::iterator indexedIt = indexedContainer.begin();
    while (it != container.end() && indexedIt != indexedContainer.end()) {
        *it = indexedIt->first;
        ++it;
        ++indexedIt;
    }
}

template<typename T, template<typename> class Compare>
void PmergeMeSort(std::deque<T, std::allocator<T> > &container,
    Compare<T> cmp) {
    std::deque<typename ValIdx<T>::type, typename ValIdx<T>::alloc>
        indexedContainer(container.size());
    for (std::size_t i = 0; i < container.size(); ++i) {
        indexedContainer[i] = std::make_pair(container[i], i);
    }
    PmergeMeSortIndexed(indexedContainer, cmp);
    for (std::size_t i = 0; i < container.size(); ++i) {
        container[i] = indexedContainer[i].first;
    }
}


template<typename T, template<typename> class Compare>
void PmergeMeSort(std::vector<T, std::allocator<T> > &container,
    Compare<T> cmp) {
    std::vector<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc> indexedContainer;
    indexedContainer.reserve(container.size());
    for (std::size_t i = 0; i < container.size(); ++i) {
        indexedContainer.push_back(std::make_pair(container[i], i));
    }
    PmergeMeSortIndexed(indexedContainer, cmp);
    for (std::size_t i = 0; i < container.size(); ++i) {
        container[i] = indexedContainer[i].first;
    }
}

template<typename T, template<typename> class Compare>
void PmergeMeSort(ImplicitTreap<T> &container,
    Compare<T> cmp) {
    ImplicitTreap<typename ValIdx<T>::type> indexedContainer;
    for (std::size_t i = 0; i < container.size(); ++i) {
        indexedContainer.insert(i, std::make_pair(container[i], i));
    }
    PmergeMeSortIndexed(indexedContainer, cmp);
    for (std::size_t i = 0; i < container.size(); ++i) {
        typename ValIdx<T>::type entry = indexedContainer[i];
        container[i] = entry.first;
    }
}
