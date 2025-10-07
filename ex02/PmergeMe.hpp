#pragma once

#include <algorithm>
#include <iterator>
#include <ex02/type_trait.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/detail/utils.hpp>
#include <ex02/detail/1_cmp_neighbor.hpp>
#include <ex02/detail/2_reorder_pairs.hpp>
#include <ex02/detail/3_create_main_chain.hpp>
#include <ex02/detail/4_insert_into_main_chain.hpp>
#include <ex02/detail/5_write_back.hpp>

template<typename T, template<typename, typename> class Container, typename Compare>
void PmergeMeSort(Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > pairs;
    Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > bigger;

    // -> 1_cmp_neighbor.hpp
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // pairs.reserve(bigger_size);
            // bigger.reserve(num_elements);
        }
        for (std::size_t i = 0; i < bigger_size; ++i) {
            std::pair<T, std::size_t> first = container[2 * i];
            std::pair<T, std::size_t> second = container[2 * i + 1];
            if (cmp(first.first, second.first)) {
                pairs.push_back(std::make_pair(second, first));
                bigger.push_back(std::make_pair(second.first, i));
            } else {
                pairs.push_back(std::make_pair(first, second));
                bigger.push_back(std::make_pair(first.first, i));
            }
        }
    } else {
        // neither vector nor deque
    }

    bool hasStraggler = (num_elements % 2) == 1;
    std::pair<T, std::size_t> straggler = std::make_pair(T(), 0);
    if (hasStraggler) {
        straggler = container.back();
    }

    PmergeMeSort(bigger, cmp);

    // -> 2_reorder_pairs.hpp
    Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > orderedPairs(pairs.size());
    for (std::size_t i = 0; i < pairs.size(); ++i) {
        orderedPairs[i] = pairs[bigger[i].second];
    }
    pairs.swap(orderedPairs);

    Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > mainChain;

    // -> 3_create_main_chain.hpp
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
    // -> 4_insert_into_main_chain.hpp
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
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
                typename Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > >::iterator end_iterator = mainChain.end();
                if (pairs[i - 1].first.first != T()) {
                    std::size_t bigger_cnt = 0;
                    for (std::size_t j = 0; j < mainChain.size(); ++j) {
                        if (mainChain[j].second) {
                            ++bigger_cnt;
                        }
                        if (bigger_cnt == i) {
                            end_iterator = mainChain.begin() + j; // これができるのはvectorかdequeの場合のみ
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
    } else {
        // neither vector nor deque
    }

    // -> 5_write_back.hpp
    // if (is_deque<Container<T, std::allocator<T> > >::value ||
    //         is_vector<Container<T, std::allocator<T> > >::value) {
    //     for (std::size_t i = 0; i < mainChain.size(); ++i) {
    //         container[i] = mainChain[i].first;
    //     }
    // } else {
        // neither vector nor deque
    // }
    writeBack(container, mainChain);
}


template<typename T, template<typename, typename> class Container, typename Compare>
void PmergeMeSort(Container<T, std::allocator<T> > &container, Compare cmp) {
    Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > indexedContainer;
    for (std::size_t i = 0; i < container.size(); ++i) {
        indexedContainer.push_back(std::make_pair(container[i], i));
    }
    PmergeMeSort(indexedContainer, cmp);
    for (std::size_t i = 0; i < container.size(); ++i) {
        container[i] = indexedContainer[i].first;
    }
}
