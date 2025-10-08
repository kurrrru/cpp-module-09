#pragma once

#include <iterator>
#include <memory>
#include <utility>

template<typename T, template<typename, typename> class Container, typename Compare>
void insertIntoMainChain(Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > &mainChain,
        bool hasStraggler, const std::pair<T, std::size_t> &straggler) {
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
}
