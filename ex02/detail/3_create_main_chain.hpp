#pragma once

#include <memory>
#include <utility>

#include <ex02/type_trait.hpp>

template<typename T, template<typename, typename> class Container>
void createMainChain(const Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > &mainChain) {
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // mainChain.reserve(num_elements);
        }
        std::pair<T, std::size_t> b_1 = pairs[0].second;
        mainChain.push_back(std::make_pair(b_1, false));
        for (std::size_t i = 0; i < pairs.size(); ++i) {
            mainChain.push_back(std::make_pair(pairs[i].first, true));
        }
    } else {
        // neither vector nor deque
    }
}
