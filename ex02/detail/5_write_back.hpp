#pragma once

#include <iterator>
#include <memory>

#include <ex02/detail/utils.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void writeBack(Container<typename ValIdx<T>::type,
    typename ValIdx<T>::alloc> &container,
    const Container<typename ValIdx<T>::btype,
    typename ValIdx<T>::balloc> &mainChain, Compare<T> cmp) {
    (void)cmp;
    typename Container<typename ValIdx<T>::type, typename ValIdx<T>::alloc>
        ::iterator it = container.begin();
    typename Container<typename ValIdx<T>::btype, typename ValIdx<T>::balloc>
        ::const_iterator it_main = mainChain.begin();
    while (it_main != mainChain.end()) {
        *it++ = it_main->first;
        ++it_main;
    }
}
