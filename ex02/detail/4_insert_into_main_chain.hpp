#pragma once

#include <deque>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/detail/utils.hpp>

template<typename T, template<typename, typename> class Container, typename Compare>
void insertIntoMainChain(Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > &mainChain,
        bool hasStraggler, const std::pair<T, std::size_t> &straggler, Compare cmp) {
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
            typename Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > >::iterator pair_it = pairs.begin();
            std::advance(pair_it, i - 1);
            const std::pair<T, std::size_t> &insert_value = pair_it->second;
            typename Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > >::iterator end_iterator = mainChain.end();
            if (pair_it->first.first != T()) {
                std::size_t bigger_cnt = 0;
                for (typename Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > >::iterator it = mainChain.begin(); it != mainChain.end(); ++it) {
                    if (it->second) {
                        ++bigger_cnt;
                    }
                    if (bigger_cnt == i) {
                        end_iterator = it;
                        break;
                    }
                }
            }
            typename Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > >::iterator insert_position = std::lower_bound(
                mainChain.begin(), end_iterator, std::make_pair(insert_value, false),
                ComparePairFirstFirst<T, Compare>()
            );
            mainChain.insert(insert_position, std::make_pair(insert_value, false));
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}

// vector
template<typename T, typename Compare>
void insertIntoMainChain(std::vector<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        std::vector<std::pair<std::pair<T, std::size_t>, bool> > &mainChain,
        bool hasStraggler, const std::pair<T, std::size_t> &straggler, Compare cmp) {
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
            const std::pair<T, std::size_t> &insert_value = pairs[i - 1].second;
            typename std::vector<std::pair<std::pair<T, std::size_t>, bool> >::iterator end_iterator = mainChain.end();
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
            typename std::vector<std::pair<std::pair<T, std::size_t>, bool> >::iterator insert_position = std::lower_bound(
                mainChain.begin(), end_iterator, std::make_pair(insert_value, false),
                ComparePairFirstFirst<T, Compare>()
            );
            mainChain.insert(insert_position, std::make_pair(insert_value, false));
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}

// ImplicitTreap
template<typename T, typename Compare>
void insertIntoMainChain(ImplicitTreap<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > &pairs,
        ImplicitTreap<std::pair<std::pair<T, std::size_t>, bool> > &mainChain,
        bool hasStraggler, const std::pair<T, std::size_t> &straggler, Compare cmp) {
    (void)cmp;
    if (hasStraggler) {
        pairs.insert(pairs.end(), std::make_pair(std::make_pair(T(), 0), straggler));
    }

    typedef ImplicitTreap<int> FlagTreap;
    FlagTreap biggerFlags;
    for (std::size_t idx = 0; idx < mainChain.size(); ++idx) {
        std::pair<std::pair<T, std::size_t>, bool> entry = mainChain[idx];
        biggerFlags.insert(idx, entry.second ? 1 : 0);
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
            std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > currentPair = pairs[i - 1];
            const std::pair<T, std::size_t> &insert_value = currentPair.second;

            std::size_t searchLimit = mainChain.size();
            if (currentPair.first.first != T()) {
                typename FlagTreap::value_type target = static_cast<typename FlagTreap::value_type>(i);
                std::size_t targetIndex = biggerFlags.query_lower_bound(0, biggerFlags.size(), target);
                if (targetIndex < biggerFlags.size()) {
                    searchLimit = targetIndex;
                }
            }

            std::size_t left = 0;
            std::size_t right = searchLimit;
            std::pair<std::pair<T, std::size_t>, bool> probe = std::make_pair(insert_value, false);
            while (left < right) {
                std::size_t mid = left + (right - left) / 2;
                std::pair<std::pair<T, std::size_t>, bool> midValue = mainChain[mid];
                if (comparator(midValue, probe)) {
                    left = mid + 1;
                } else {
                    right = mid;
                }
            }

            typename ImplicitTreap<std::pair<std::pair<T, std::size_t>, bool> >::iterator insert_position = mainChain.begin() + static_cast<typename ImplicitTreap<std::pair<std::pair<T, std::size_t>, bool> >::difference_type>(left);
            mainChain.insert(insert_position, std::make_pair(insert_value, false));
            biggerFlags.insert(biggerFlags.begin() + static_cast<typename FlagTreap::difference_type>(left), 0);
        }
        if (jacobsthal.first >= pairs.size()) {
            break;
        }
    }
}
