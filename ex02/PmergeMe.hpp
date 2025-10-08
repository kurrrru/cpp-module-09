#pragma once

#include <utility>
#include <algorithm>
#include <iterator>
#include <ex02/type_trait.hpp>
#include <ex02/detail/details.hpp>

template<typename T, template<typename, typename> class Container, typename Compare>
void PmergeMeSort(Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }
    std::size_t num_elements = container.size();

    Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > pairs;
    Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > bigger;

    // 1_cmp_neighbor.hpp
    cmpNeighbor(container, pairs, bigger, cmp);

    bool hasStraggler = (num_elements % 2) == 1;
    std::pair<T, std::size_t> straggler = std::make_pair(T(), 0);
    if (hasStraggler) {
        straggler = container.back();
    }

    PmergeMeSort(bigger, cmp);

    // 2_reorder_pairs.hpp
    reorderPairs(pairs, bigger);

    Container<std::pair<std::pair<T, std::size_t>, bool>, std::allocator<std::pair<std::pair<T, std::size_t>, bool> > > mainChain;

    // 3_create_main_chain.hpp
    createMainChain(pairs, mainChain);

    // 4_insert_into_main_chain.hpp
    insertIntoMainChain<T, Container, Compare>(pairs, mainChain, hasStraggler, straggler);

    // 5_write_back.hpp
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
