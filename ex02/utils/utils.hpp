#pragma once

#include <iostream>
#include <memory>

template<typename T, template<typename, typename> class Container>
void printContainer(const Container<T, std::allocator<T> > &container,
    std::size_t width) {
    typename Container<T, std::allocator<T> >
        ::const_iterator it = container.begin();
    if (container.size() <= 10) {
        for (; it != container.end(); ++it) {
            std::cout << " " << std::setw(width) << *it;
        }
        std::cout << std::endl;
    } else {
        for (std::size_t i = 0; i < 5 && it != container.end(); ++i, ++it) {
            std::cout << " " << std::setw(width) << *it;
        }
        std::cout << " [...]" << std::endl;
    }
}
