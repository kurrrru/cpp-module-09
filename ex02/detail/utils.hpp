#pragma once

#include <memory>
#include <utility>

void nextJacobsthal(std::pair<std::size_t, std::size_t> &j);

template<typename T>
struct ValIdx {
    typedef std::pair<T, std::size_t>            type;
    typedef std::allocator<type>                 alloc;
    typedef std::pair<typename ValIdx<T>::type,
        typename ValIdx<T>::type>                ptype;
    typedef std::allocator<ptype>                palloc;
    typedef std::pair<typename ValIdx<T>::type,
        bool>                                    btype;
    typedef std::allocator<btype>                balloc;
};

template <typename T, template<typename> class Compare>
struct ComparePairFirstFirst {
    bool operator()(const typename ValIdx<T>::btype &a,
        const typename ValIdx<T>::btype &b) {
        return Compare<T>()(a.first.first, b.first.first);
    }
};
