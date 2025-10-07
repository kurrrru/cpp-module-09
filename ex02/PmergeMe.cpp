#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>

#include <ex02/PmergeMe.hpp>
#include <ex02/type_trait.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/compare/CGreater.hpp>



int main() {
    std::vector<int> vec;
    vec.push_back(34);
    vec.push_back(7);
    vec.push_back(23);
    vec.push_back(32);
    vec.push_back(5);
    vec.push_back(62);
    std::deque<int> deq;
    deq.push_back(34);
    deq.push_back(7);
    deq.push_back(23);
    deq.push_back(32);
    deq.push_back(5);
    deq.push_back(62);

    std::cout << "Original vector: ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Original deque: ";
    for (size_t i = 0; i < deq.size(); ++i) {
        std::cout << deq[i] << " ";
    }
    std::cout << std::endl;

    // mergeSort(vec, comparer::CLess<int>());
    // mergeSort(deq, comparer::CGreater<int>());

    PmergeMeSort(vec, comparer::CLess<int>());
    PmergeMeSort(deq, comparer::CGreater<int>());

    // std::sort(vec.begin(), vec.end(), comparer::CLess<int>{});
    // std::sort(deq.begin(), deq.end(), comparer::CGreater<int>{});

    std::cout << "Sorted vector (ascending): ";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Sorted deque (descending): ";
    for (size_t i = 0; i < deq.size(); ++i) {
        std::cout << deq[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "Number of comparisons in vector sort: " << comparer::CLess<int>::getcnt() << std::endl;
    std::cout << "Number of comparisons in deque sort: " << comparer::CGreater<int>::getcnt() << std::endl;

    std::vector<int> empty_vec;
    for (int i = 1; i <= 7; ++i) {
        empty_vec.push_back(i);
    }
    std::size_t cnt_cmp = 0;
    do {
        std::vector<int> cpy = empty_vec;
        comparer::CLess<int>::reset();
        PmergeMeSort(cpy, comparer::CLess<int>());
        bool is_sorted = true;
        for (size_t i = 1; i < cpy.size(); ++i) {
            if (cpy[i - 1] > cpy[i]) {
                is_sorted = false;
                break;
            }
        }
        if (!is_sorted) {
            std::cout << "Sorting failed for permutation: ";
            for (size_t i = 0; i < empty_vec.size(); ++i) {
                std::cout << empty_vec[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "Result: ";
            for (size_t i = 0; i < cpy.size(); ++i) {
                std::cout << cpy[i] << " ";
            }
            std::cout << std::endl;
        }
        else {
            cnt_cmp = std::max(cnt_cmp, static_cast<std::size_t>(comparer::CLess<int>::getcnt()));
            // std::cout << "Sorted successfully with " << comparer::CLess<int>::getcnt() << " comparisons for permutation: ";
            // for (size_t i = 0; i < empty_vec.size(); ++i) {
            //     std::cout << empty_vec[i] << " ";
            // }
            // std::cout << std::endl;
        }
    } while (std::next_permutation(empty_vec.begin(), empty_vec.end()));

    std::cout << "Maximum number of comparisons: " << cnt_cmp << std::endl;

    std::vector<std::string> s;
    s.push_back("Eagle");
    s.push_back("Apple");
    s.push_back("Cat");
    s.push_back("Banana");
    s.push_back("Dog");
    PmergeMeSort(s, comparer::CLess<std::string>());
    std::cout << "Sorted strings (ascending): ";
    for (size_t i = 0; i < s.size(); ++i) {
        std::cout << s[i] << " ";
    }
    std::cout << std::endl;

        std::vector<double> d(5000);
    for (size_t i = 0; i < d.size(); ++i) {
        d[i] = std::abs(static_cast<double>((d.size() + i) * 647323 % 50001) / 3.0) + 0.5;
    }
    PmergeMeSort(d, comparer::CLess<double>());
    bool sorted = true;
    for (size_t i = 1; i < d.size(); ++i) {
        if (d[i - 1] > d[i]) {
            sorted = false;
            break;
        }
    }
    if (sorted) {
        std::cout << "Large vector sorted successfully." << std::endl;
    } else {
        std::cout << "Large vector sorting failed." << std::endl;
    }
    for (size_t i = 0; i < 10; ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << "... ";
    for (size_t i = d.size() - 10; i < d.size(); ++i) {
        std::cout << d[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
