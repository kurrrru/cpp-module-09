#pragma once

#include <algorithm>
#include <iterator>
#include <memory>

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
