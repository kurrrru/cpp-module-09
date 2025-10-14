#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include <ex02/detail/details.hpp>

template<typename T, template<typename, typename> class Container,
    template<typename> class Compare>
void PmergeMeSortIndexed(Container<typename ValIdx<T>::type,
        typename ValIdx<T>::alloc> &container, Compare<T> cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t num_elements = container.size();

    Container<typename ValIdx<T>::ptype, typename ValIdx<T>::palloc> pairs;
    Container<typename ValIdx<T>::type, typename ValIdx<T>::alloc> bigger;

    // 1_cmp_neighbor.hpp
    cmpNeighbor(container, pairs, bigger, cmp);

    bool hasStraggler = (num_elements % 2) == 1;
    typename ValIdx<T>::type straggler = std::make_pair(T(), 0);
    if (hasStraggler) {
        straggler = container.back();
    }

    PmergeMeSortIndexed(bigger, cmp);

    // 2_reorder_pairs.hpp
    reorderPairs(pairs, bigger, cmp);

    Container<typename ValIdx<T>::btype, typename ValIdx<T>::balloc> mainChain;

    // 3_create_main_chain.hpp
    createMainChain(pairs, mainChain, cmp);

    // 4_insert_into_main_chain.hpp
    insertIntoMainChain(pairs, mainChain, hasStraggler, straggler, cmp);

    // 5_write_back.hpp
    writeBack(container, mainChain, cmp);
}
