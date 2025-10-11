#include "ImplicitTreap.hpp"

#include <iostream>
#include <vector>

int main() {
    ImplicitTreap<int> treap;
    std::vector<int> values = {5, 3, 8, 1, 4, 7, 9, 2, 6};
    for (std::size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << " ";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < values.size(); ++i) {
        treap.insert(i, values[i]);
    }

    std::cout << "Initial treap (in-order): ";
    for (size_t i = 0; i < treap.size(); ++i) {
        std::cout << treap[i] << " ";
    }
    std::cout << std::endl;

    int l = 2, r = 5;
    int sum = treap.query(l, r);
    std::cout << "Sum of elements from index " << l << " to " << r - 1 << ": " << sum << std::endl;

    int update_val = 10;
    treap.update(l, r, update_val);
    std::cout << "Treap after adding " << update_val << " to elements from index " << l << " to " << r - 1 << ": ";
    for (size_t i = 0; i < treap.size(); ++i) {
        std::cout << treap[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Treap after setting index 0 to 20: ";
    for (size_t i = 0; i < treap.size(); ++i) {
        std::cout << treap[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}