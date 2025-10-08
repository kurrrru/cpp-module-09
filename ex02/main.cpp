#include <algorithm>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <ex02/PmergeMe.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/compare/CGreater.hpp>
#include <ex02/clock/clock.hpp>
#include <toolbox/string.hpp>

namespace {
    std::size_t longest_integer_width(const std::vector<int>& vec) {
        int max_val = *std::max_element(vec.begin(), vec.end());
        return toolbox::to_string(max_val).size();
    }

    void print_vec(const std::vector<int>& vec, std::size_t width) {
        if (vec.size() <= 10) {
            for (size_t i = 0; i < vec.size(); ++i) {
                std::cout << std::setw(width) << vec[i] << " ";
            }
            std::cout << std::endl;
        } else {
            for (size_t i = 0; i < 4; ++i) {
                std::cout << std::setw(width) << vec[i] << " ";
            }
            std::cout << "[...]" << std::endl;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Error: At least one positive integer argument is required." << std::endl;
        return 1;
    }
    std::vector<int> vec(argc - 1);
    try {
        for (int i = 1; i < argc; ++i) {
            int num = toolbox::stoi(argv[i]);
            if (num <= 0) {
                std::cerr << "Error: All arguments must be positive integers." << std::endl;
                return 1;
            }
            vec[i - 1] = num;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Conversion failed: " << e.what() << std::endl;
        return 1;
    }
    std::deque<int> deq(vec.begin(), vec.end());
    std::size_t width = longest_integer_width(vec);
    std::cout << "Before: ";
    print_vec(vec, width);
    int64 start_vec, end_vec;
    start_vec = get_microseconds();
    PmergeMeSort(vec, comparer::CLess<int>());
    end_vec = get_microseconds();
    int64 start_deq, end_deq;
    start_deq = get_microseconds();
    PmergeMeSort(deq, comparer::CLess<int>());
    end_deq = get_microseconds();
    bool vec_sorted = true;
    for (size_t i = 1; i < vec.size(); ++i) {
        if (vec[i - 1] > vec[i]) {
            vec_sorted = false;
            break;
        }
    }
    if (!vec_sorted) {
        std::cerr << "Error: Vector sorting failed." << std::endl;
        return 1;
    }
    bool deq_sorted = true;
    for (size_t i = 1; i < deq.size(); ++i) {
        if (deq[i - 1] > deq[i]) {
            deq_sorted = false;
            break;
        }
    }
    if (!deq_sorted) {
        std::cerr << "Error: Deque sorting failed." << std::endl;
        return 1;
    }
    std::cout << "After:  ";
    print_vec(vec, width);
    std::cout << "Time to process a range of " << std::setw(4) << vec.size()
            << " elements with std::vector : " << std::setw(10)
            << (end_vec - start_vec) << " us" << std::endl;
    std::cout << "Time to process a range of " << std::setw(4) << vec.size()
            << " elements with std::deque  : " << std::setw(10)
            << (end_deq - start_deq) << " us" << std::endl;


    return 0;
}