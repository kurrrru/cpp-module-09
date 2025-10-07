#pragma once

#include <iterator>
#include <memory>

template<typename T, template<typename, typename> class Container>
void writeBack(Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &container,
        const Container<std::pair<std::pair<T, std::size_t>, bool>,
        std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > &mainChain) {
    typename Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > >::iterator it = container.begin();
    typename Container<std::pair<std::pair<T, std::size_t>, bool>,
        std::allocator<std::pair<std::pair<T, std::size_t>, bool> > >::const_iterator
        it_main = mainChain.begin();
    while (it_main != mainChain.end()) {
        *it++ = it_main->first;
        ++it_main;
    }
}
