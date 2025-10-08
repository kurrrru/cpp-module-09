#pragma once

#include <memory>
#include <utility>

template<typename T, template<typename, typename> class Container>
void createMainChain(const Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > &mainChain) {
    typename Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > >::const_iterator it = pairs.begin();
    if (it != pairs.end()) {
        std::pair<T, std::size_t> b_1 = it->second;
        mainChain.push_back(std::make_pair(b_1, false));
        for (; it != pairs.end(); ++it) {
            mainChain.push_back(std::make_pair(it->first, true));
        }
    }
}

// vectorにテンプレート特殊化する
template<typename T>
void createMainChain(const std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::vector<std::pair<std::pair<T, std::size_t>, bool> > &mainChain) {
    mainChain.reserve(pairs.size() + 1);
    std::pair<T, std::size_t> b_1 = pairs[0].second;
    mainChain.push_back(std::make_pair(b_1, false));
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(std::make_pair(pairs[i].first, true));
    }
}

// dequeにテンプレート特殊化する
template<typename T>
void createMainChain(const std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::deque<std::pair<std::pair<T, std::size_t>, bool> > &mainChain) {
    mainChain.resize(pairs.size() + 1);
    std::pair<T, std::size_t> b_1 = pairs[0].second;
    mainChain[0] = std::make_pair(b_1, false);
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        mainChain[i + 1] = std::make_pair(pairs[i].first, true);
    }
}
