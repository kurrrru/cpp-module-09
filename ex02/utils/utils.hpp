#pragma once

#include <iostream>
#include <memory>

template<typename T, template<typename, typename> class Container>
void printContainer(const Container<T, std::allocator<T> > &container) {
    typename Container<T, std::allocator<T> >::const_iterator it = container.begin();
    if (it != container.end()) {
        std::cout << it->first;
        ++it;
    }
    for (; it != container.end(); ++it) {
        std::cout << " " << it->first;
    }
    std::cout << std::endl;
}
