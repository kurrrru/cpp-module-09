#pragma once

#include <algorithm>
#include <iterator>
#include <ex02/type_trait.hpp>
#include <ex02/compare/CLess.hpp>

void nextJacobsthal(std::pair<std::size_t, std::size_t> &j) {
    std::swap(j.first, j.second);
    j.first = j.first * 2 + j.second;
}


template <typename T, typename Compare>
struct ComparePairFirst {
    bool operator()(const std::pair<T, bool> &a, const std::pair<T, bool> &b) {
        return Compare()(a.first, b.first);
    }
};


template<typename T, template<typename, typename> class Container, typename Compare>
void mergeSort(Container<T, std::allocator<T> > &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }

    typename Container<T, std::allocator<T> >::iterator mid = container.begin() + container.size() / 2;
    Container<T, std::allocator<T> > left(container.begin(), mid);
    Container<T, std::allocator<T> > right(mid, container.end());

    mergeSort(left, cmp);
    mergeSort(right, cmp);

    typename Container<T, std::allocator<T> >::iterator it = container.begin();
    typename Container<T, std::allocator<T> >::iterator it_left = left.begin();
    typename Container<T, std::allocator<T> >::iterator it_right = right.begin();

    while (it_left != left.end() && it_right != right.end()) {
        if (cmp(*it_left, *it_right)) {
            *it++ = *it_left++;
        } else {
            *it++ = *it_right++;
        }
    }

    while (it_left != left.end()) {
        *it++ = *it_left++;
    }

    while (it_right != right.end()) {
        *it++ = *it_right++;
    }
}

template<typename T, template<typename, typename> class Container, typename Compare>
void PmergeMeSort(Container<T, std::allocator<T> > &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    Container<std::pair<T, T>, std::allocator<std::pair<T, T> > > pairs;
    Container<T, std::allocator<T> > bigger;

    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // pairs.reserve(bigger_size);
            // bigger.reserve(num_elements);
        }
        for (std::size_t i = 0; i < bigger_size; ++i) {
            T first = container[2 * i];
            T second = container[2 * i + 1];
            if (cmp(first, second)) {
                pairs.push_back(std::make_pair(second, first));
                bigger.push_back(second);
            } else {
                pairs.push_back(std::make_pair(first, second));
                bigger.push_back(first);
            }
        }
    } else {
        // neither vector nor deque
    }

    bool hasStraggler = (num_elements % 2) == 1;
    T straggler = T();
    if (hasStraggler) {
        straggler = container.back();
    }

    PmergeMeSort(bigger, cmp);

    Container<std::pair<T, T>, std::allocator<std::pair<T, T> > > orderedPairs;
    Container<bool, std::allocator<bool> > used;
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // orderedPairs.reserve(pairs.size());
        }
        used.resize(pairs.size(), false);
        for (std::size_t i = 0; i < bigger.size(); ++i) {
            for (std::size_t j = 0; j < pairs.size(); ++j) {
                if (!used[j] && pairs[j].first == bigger[i]) {
                    orderedPairs.push_back(pairs[j]);
                    used[j] = true;
                    break;
                }
            }
        }
    } else {
        // neither vector nor deque
    }
    pairs.swap(orderedPairs);

    Container<std::pair<T, bool>, std::allocator<std::pair<T, bool> > > mainChain;

    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // mainChain.reserve(num_elements);
        }
        T b_1 = pairs[0].second;
        pairs[0].second = T();
        mainChain.push_back(std::make_pair(b_1, false));
        for (std::size_t i = 0; i < pairs.size(); ++i) {
            mainChain.push_back(std::make_pair(pairs[i].first, true));
        }
    } else {
        // neither vector nor deque
    }
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (hasStraggler) {
            pairs.push_back(std::make_pair(T(), straggler));
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
                const T &insert_value = pairs[i - 1].second;
                typename Container<std::pair<T, bool>, std::allocator<std::pair<T, bool> > >::iterator end_iterator = mainChain.end();
                if (pairs[i - 1].first != T()) {
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
                typename Container<std::pair<T, bool>, std::allocator<std::pair<T, bool> > >::iterator insert_position = std::lower_bound(
                    mainChain.begin(), end_iterator, std::make_pair(insert_value, false),
                    ComparePairFirst<T, Compare>()
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
    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        for (std::size_t i = 0; i < mainChain.size(); ++i) {
            container[i] = mainChain[i].first;
        }
    } else {
        // neither vector nor deque
    }
}
