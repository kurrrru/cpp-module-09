#pragma once

#include <deque>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/detail/utils.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void insertIntoMainChain(Container<typename ValIdx<T>::ptype,
    typename ValIdx<T>::palloc> &pairs,
    Container<typename ValIdx<T>::btype,
    typename ValIdx<T>::balloc> &mainChain,
    bool hasStraggler, const typename ValIdx<T>::type &straggler,
    Compare<T> cmp) {
    (void)cmp;
    if (hasStraggler) {
        pairs.push_back(std::make_pair(std::make_pair(T(), 0), straggler));
    }
    std::pair<std::size_t, std::size_t> jacobsthal = std::make_pair(1, 1);
    while (true) {
        nextJacobsthal(jacobsthal);
        for (std::size_t i = jacobsthal.first; i > jacobsthal.second; --i) {
            if (i > pairs.size()) {
                i = pairs.size();
            }
            if (i <= 1) {
                break;
            }
            typename Container<typename ValIdx<T>::ptype,
                typename ValIdx<T>::palloc>::iterator pair_it = pairs.begin();
            std::advance(pair_it, i - 1);
            const typename ValIdx<T>::type &insert_value = pair_it->second;
            typename Container<typename ValIdx<T>::btype,
                typename ValIdx<T>::balloc>
                ::iterator end_iterator = mainChain.end();
            if (pair_it->first.first != T()) {
                std::size_t bigger_cnt = 0;
                for (typename Container<typename ValIdx<T>::btype,
                    typename ValIdx<T>::balloc>
                    ::iterator it = mainChain.begin();
                    it != mainChain.end(); ++it) {
                    if (it->second) {
                        ++bigger_cnt;
                    }
                    if (bigger_cnt == i) {
                        end_iterator = it;
                        break;
                    }
                }
            }
            typename Container<typename ValIdx<T>::btype,
                typename ValIdx<T>::balloc>
                ::iterator insert_position = std::lower_bound(
                mainChain.begin(), end_iterator,
                std::make_pair(insert_value, false),
                ComparePairFirstFirst<T, Compare>());
            mainChain.insert(insert_position,
                std::make_pair(insert_value, false));
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}

// vector
template<typename T, template<typename> class Compare>
void insertIntoMainChain(std::vector<typename ValIdx<T>::ptype> &pairs,
        std::vector<typename ValIdx<T>::btype> &mainChain,
        bool hasStraggler, const typename ValIdx<T>::type &straggler,
        Compare<T> cmp) {
    (void)cmp;
    if (hasStraggler) {
        pairs.push_back(std::make_pair(std::make_pair(T(), 0), straggler));
    }
    std::pair<std::size_t, std::size_t> jacobsthal = std::make_pair(1, 1);
    while (true) {
        nextJacobsthal(jacobsthal);
        for (std::size_t i = jacobsthal.first; i > jacobsthal.second; --i) {
            if (i > pairs.size()) {
                i = pairs.size();
            }
            if (i <= 1) {
                break;
            }
            const typename ValIdx<T>::type &insert_value = pairs[i - 1].second;
            typename std::vector<typename ValIdx<T>::btype>
                ::iterator end_iterator = mainChain.end();
            if (pairs[i - 1].first.first != T()) {
                std::size_t bigger_cnt = 0;
                for (std::size_t j = 0; j < mainChain.size(); ++j) {
                    if (mainChain[j].second) {
                        ++bigger_cnt;
                    }
                    if (bigger_cnt == i) {
                        end_iterator = mainChain.begin() + j;
                        break;
                    }
                }
            }
            typename std::vector<typename ValIdx<T>::btype>
                ::iterator insert_position = std::lower_bound(
                mainChain.begin(), end_iterator,
                std::make_pair(insert_value, false),
                ComparePairFirstFirst<T, Compare>());
            mainChain.insert(insert_position,
                std::make_pair(insert_value, false));
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}

// ImplicitTreap
template<typename T, template<typename> class Compare>
void insertIntoMainChain(ImplicitTreap<typename ValIdx<T>::ptype> &pairs,
        ImplicitTreap<typename ValIdx<T>::btype> &mainChain,
        bool hasStraggler, const typename ValIdx<T>::type &straggler,
        Compare<T> cmp) {
    (void)cmp;
    if (hasStraggler) {
        pairs.insert(pairs.end(), std::make_pair(std::make_pair(T(), 0),
            straggler));
    }

    typedef ImplicitTreap<int> FlagTreap;
    FlagTreap biggerFlags;
    for (std::size_t idx = 0; idx < mainChain.size(); ++idx) {
        biggerFlags.insert(idx, idx > 0 ? 1 : 0);
    }

    ComparePairFirstFirst<T, Compare> comparator;
    std::pair<std::size_t, std::size_t> jacobsthal = std::make_pair(1, 1);
    while (true) {
        nextJacobsthal(jacobsthal);
        for (std::size_t i = jacobsthal.first; i > jacobsthal.second; --i) {
            if (i > pairs.size()) {
                i = pairs.size();
            }
            if (i <= 1) {
                break;
            }
            typename ValIdx<T>::ptype currentPair = pairs[i - 1];
            const typename ValIdx<T>::type &insert_value = currentPair.second;

            std::size_t searchLimit = mainChain.size();
            if (currentPair.first.first != T()) {
                typename FlagTreap::value_type
                    target = static_cast<typename FlagTreap::value_type>(i);
                std::size_t targetIndex = biggerFlags.query_lower_bound(0,
                    biggerFlags.size(), target);
                if (targetIndex < biggerFlags.size()) {
                    searchLimit = targetIndex;
                }
            }

            std::size_t left = 0;
            std::size_t right = searchLimit;
            typename ValIdx<T>::btype
                probe = std::make_pair(insert_value, false);
            while (left < right) {
                std::size_t mid = left + (right - left) / 2;
                typename ValIdx<T>::btype midValue = mainChain[mid];
                if (comparator(midValue, probe)) {
                    left = mid + 1;
                } else {
                    right = mid;
                }
            }

            typename ImplicitTreap<typename ValIdx<T>::btype>
                ::iterator insert_position = mainChain.begin()
                + static_cast<typename ImplicitTreap<std::pair<std::pair<T,
                std::size_t>, bool> >::difference_type>(left);
            mainChain.insert(insert_position,
                std::make_pair(insert_value, false));
            biggerFlags.insert(biggerFlags.begin()
                + static_cast<typename FlagTreap::difference_type>(left), 0);
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}
