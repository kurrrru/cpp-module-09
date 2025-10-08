#pragma once

template<typename T, template<typename, typename> class Container, typename Compare>
void cmpNeighbor(const Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &container,
        Container<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> >, std::allocator<std::pair<std::pair<T, std::size_t>, std::pair<T, std::size_t> > > > &pairs,
        Container<std::pair<T, std::size_t>, std::allocator<std::pair<T, std::size_t> > > &bigger,
        Compare cmp) {
    std::size_t num_elements = container.size();
    std::size_t bigger_size = num_elements / 2;

    if (is_deque<Container<T, std::allocator<T> > >::value ||
            is_vector<Container<T, std::allocator<T> > >::value) {
        if (is_vector<Container<T, std::allocator<T> > >::value) {
            // pairs.reserve(bigger_size);
            // bigger.reserve(num_elements);
        }
        for (std::size_t i = 0; i < bigger_size; ++i) {
            std::pair<T, std::size_t> first = container[2 * i];
            std::pair<T, std::size_t> second = container[2 * i + 1];
            if (cmp(first.first, second.first)) {
                pairs.push_back(std::make_pair(second, first));
                bigger.push_back(std::make_pair(second.first, i));
            } else {
                pairs.push_back(std::make_pair(first, second));
                bigger.push_back(std::make_pair(first.first, i));
            }
        }
    } else {
        // neither vector nor deque
    }
}
