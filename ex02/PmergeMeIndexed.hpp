#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>

#include <ex02/detail/details.hpp>

template<typename T, template<typename, typename> class Container,
    typename Compare>
void PmergeMeSortIndexed(Container<std::pair<T, std::size_t>,
        std::allocator<std::pair<T, std::size_t> > > &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t num_elements = container.size();

    typedef std::pair<T, std::size_t> IndexedValue;
    Container<std::pair<IndexedValue, IndexedValue>,
        std::allocator<std::pair<IndexedValue, IndexedValue> > > pairs;
    Container<IndexedValue, std::allocator<IndexedValue> > bigger;

    // 1_cmp_neighbor.hpp
    cmpNeighbor(container, pairs, bigger, cmp);

    bool hasStraggler = (num_elements % 2) == 1;
    IndexedValue straggler = std::make_pair(T(), 0);
    if (hasStraggler) {
        straggler = container.back();
    }

    PmergeMeSortIndexed(bigger, cmp);

    // 2_reorder_pairs.hpp
    reorderPairs(pairs, bigger);

    Container<std::pair<IndexedValue, bool>,
        std::allocator<std::pair<IndexedValue, bool> > > mainChain;

    // 3_create_main_chain.hpp
    createMainChain(pairs, mainChain);

    // 4_insert_into_main_chain.hpp
    insertIntoMainChain(pairs, mainChain, hasStraggler, straggler, cmp);

    // 5_write_back.hpp
    writeBack(container, mainChain);
}
