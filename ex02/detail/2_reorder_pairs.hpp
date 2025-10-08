#pragma once

#include <memory>
#include <utility>

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

// vectorにテンプレート特殊化する
template<typename T>
void reorderPairs(std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        const std::vector<std::pair<T, std::size_t> > &bigger) {
    std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > orderedPairs;
    orderedPairs.reserve(pairs.size());
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        orderedPairs.push_back(pairs[bigger[i].second]);
    }
    pairs.swap(orderedPairs);
}

// dequeにテンプレート特殊化する
template<typename T>
void reorderPairs(std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        const std::deque<std::pair<T, std::size_t> > &bigger) {
    std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > orderedPairs(pairs.size());
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        orderedPairs[i] = pairs[bigger[i].second];
    }
    pairs.swap(orderedPairs);
}
