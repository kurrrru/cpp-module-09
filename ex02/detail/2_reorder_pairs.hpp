#pragma once

#include <iterator>
#include <memory>
#include <utility>

#include <ex02/datastructure/ImplicitTreap.hpp>

template<typename T, template<typename, typename> class Container>
void reorderPairs(Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        const Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &bigger) {
    Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > orderedPairs;
    for (typename Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > >::const_iterator it = bigger.begin(); it != bigger.end(); ++it) {
        typename Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > >::const_iterator pair_it = pairs.begin();
        std::advance(pair_it, it->second);
        orderedPairs.push_back(*pair_it);
    }
    pairs.swap(orderedPairs);

}

// vector
template<typename T>
void reorderPairs(std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        const std::vector<std::pair<T, std::size_t> > &bigger) {
    std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > orderedPairs;
    orderedPairs.reserve(pairs.size());
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs.push_back(pairs[bigger[i].second]);
    }
    pairs.swap(orderedPairs);
}

// deque
template<typename T>
void reorderPairs(std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        const std::deque<std::pair<T, std::size_t> > &bigger) {
    std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > orderedPairs(pairs.size());
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs[i] = pairs[bigger[i].second];
    }
    pairs.swap(orderedPairs);
}

// ImplicitTreap
template<typename T>
void reorderPairs(ImplicitTreap<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        const ImplicitTreap<std::pair<T, std::size_t> > &bigger) {
    ImplicitTreap<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > orderedPairs;
    for (std::size_t i = 0; i < bigger.size(); ++i) {
        orderedPairs.insert(i, pairs[bigger[i].second]);
    }
    pairs.swap(orderedPairs);
}



