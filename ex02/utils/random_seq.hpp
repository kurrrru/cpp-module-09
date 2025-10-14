#pragma once

#include <memory>
#include <cstdlib>
#include <ctime>

template<typename T, template<typename, typename> class Container>
Container<T, std::allocator<T> > generateRandomSequence(std::size_t size,
    T minValue, T maxValue) {
    Container<T, std::allocator<T> > container;
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::time(NULL)));
        seeded = true;
    }
    for (std::size_t i = 0; i < size; ++i) {
        T value = minValue + (std::rand() % (maxValue - minValue + 1));
        container.push_back(value);
    }
    return container;
}
