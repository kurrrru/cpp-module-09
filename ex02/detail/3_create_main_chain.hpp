#pragma once

#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/datastructure/ImplicitTreap.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void createMainChain(const Container<typename ValIdx<T>::ptype,
        typename ValIdx<T>::palloc> &pairs,
        Container<typename ValIdx<T>::btype,
        typename ValIdx<T>::balloc> &mainChain,
        Compare<T> cmp) {
    (void)cmp;
    typename Container<typename ValIdx<T>::ptype, typename ValIdx<T>::palloc>
        ::const_iterator it = pairs.begin();
    if (it != pairs.end()) {
        typename ValIdx<T>::type b_1 = it->second;
        mainChain.push_back(std::make_pair(b_1, false));
        for (; it != pairs.end(); ++it) {
            mainChain.push_back(std::make_pair(it->first, true));
        }
    }
}

// vector
template<typename T, template<typename> class Compare>
void createMainChain(const std::vector<typename ValIdx<T>::ptype> &pairs,
    std::vector<typename ValIdx<T>::btype> &mainChain, Compare<T> cmp) {
    (void)cmp;
    mainChain.reserve(pairs.size() + 1);
    typename ValIdx<T>::type b_1 = pairs.front().second;
    mainChain.push_back(std::make_pair(b_1, false));
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(std::make_pair(pairs[i].first, true));
    }
}

// deque
template<typename T, template<typename> class Compare>
void createMainChain(const std::deque<typename ValIdx<T>::ptype> &pairs,
    std::deque<typename ValIdx<T>::btype> &mainChain, Compare<T> cmp) {
    (void)cmp;
    mainChain.resize(pairs.size() + 1);
    typename ValIdx<T>::type b_1 = pairs.front().second;
    mainChain.front() = std::make_pair(b_1, false);
    typename std::deque<typename ValIdx<T>::btype>
        ::iterator mainIt = mainChain.begin();
    typename std::deque<typename ValIdx<T>::ptype>
        ::const_iterator pairsIt = pairs.begin();
    ++mainIt;
    for (; pairsIt != pairs.end(); ++pairsIt, ++mainIt) {
        *mainIt = std::make_pair(pairsIt->first, true);
    }
}

// ImplicitTreap
template<typename T, template<typename> class Compare>
void createMainChain(const ImplicitTreap<typename ValIdx<T>::ptype> &pairs,
    ImplicitTreap<typename ValIdx<T>::btype> &mainChain, Compare<T> cmp) {
    (void)cmp;
    if (pairs.size() == 0) {
        return;
    }
    typename ValIdx<T>::type b_1 = pairs[0].second;
    mainChain.insert(0, std::make_pair(b_1, false));
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        typename ValIdx<T>::ptype currentPair = pairs[i];
        mainChain.insert(i + 1, std::make_pair(currentPair.first, true));
    }
}
