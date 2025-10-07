#pragma once

#include <iterator>
#include <memory>

template<typename T, template<typename, typename> class Container>
void writeBack(Container<T, std::allocator<T> > &container,
        const Container<std::pair<T, bool>,
        std::allocator<std::pair<T, bool> > > &mainChain) {
    typename Container<T, std::allocator<T> >::iterator it = container.begin();
    typename Container<std::pair<T, bool>,
        std::allocator<std::pair<T, bool> > >::const_iterator
        it_main = mainChain.begin();
    while (it_main != mainChain.end()) {
        *it++ = it_main->first;
        ++it_main;
    }
}
