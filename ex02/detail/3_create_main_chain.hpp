#pragma once

#include <memory>
#include <utility>

#include <ex02/datastructure/ImplicitTreap.hpp>

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

// vector
template<typename T>
void createMainChain(const std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::vector<std::pair<std::pair<T, std::size_t>, bool> > &mainChain) {
    mainChain.reserve(pairs.size() + 1);
    std::pair<T, std::size_t> b_1 = pairs.front().second;
    mainChain.push_back(std::make_pair(b_1, false));
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(std::make_pair(pairs[i].first, true));
    }
}

// deque
template<typename T>
void createMainChain(const std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::deque<std::pair<std::pair<T, std::size_t>, bool> > &mainChain) {
    mainChain.resize(pairs.size() + 1);
    std::pair<T, std::size_t> b_1 = pairs.front().second;
    mainChain.front() = std::make_pair(b_1, false);
    typename std::deque<std::pair<std::pair<T, std::size_t>, bool> >::iterator mainIt = mainChain.begin();
    typename std::deque<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > >::const_iterator pairsIt = pairs.begin();
    ++mainIt;
    for (; pairsIt != pairs.end(); ++pairsIt, ++mainIt) {
        *mainIt = std::make_pair(pairsIt->first, true);
    }
}

// ImplicitTreap
template<typename T>
void createMainChain(const ImplicitTreap<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        ImplicitTreap<std::pair<std::pair<T, std::size_t>, bool> > &mainChain) {
    if (pairs.size() == 0) {
        return;
    }
    std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > firstPair = pairs[0];
    std::pair<T, std::size_t> b_1 = firstPair.second;
    mainChain.insert(0, std::make_pair(b_1, false));
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > currentPair = pairs[i];
        mainChain.insert(i + 1, std::make_pair(currentPair.first, true));
    }
}