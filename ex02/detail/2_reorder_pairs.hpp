#pragma once

#include <memory>
#include <utility>

template<typename T, template<typename, typename> class Container>
void reorderPairs(Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        const Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &bigger) {
    Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > orderedPairs(pairs.size());
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        orderedPairs[i] = pairs[bigger[i].second];
    }
    pairs.swap(orderedPairs);
}