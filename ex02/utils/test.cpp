#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <ex02/PmergeMe.hpp>
#include <ex02/clock/clock.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/compare/CGreater.hpp>
#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/utils/random_seq.hpp>

void test() {
    std::size_t max_size = 8;
    for (std::size_t size = 1; size <= max_size; ++size) {
        std::vector<int> empty_vec;
        for (std::size_t i = 1; i <= size; ++i) {
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

        std::cout << "Maximum number of comparisons: " << cnt_cmp << " for size " << size << std::endl;
    }

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

    std::list<int> lst;

    std::list<double> ld;
    for (size_t i = 0; i < 3000; ++i) {
        ld.push_back(std::abs(static_cast<double>((ld.size() + i) * 647323 % 50001) / 3.0) + 0.5);
    }
    PmergeMeSort(ld, comparer::CLess<double>());
    sorted = true;
    for (std::list<double>::iterator it = ld.begin(); it != ld.end(); it++) {
        std::list<double>::iterator next_it = it;
        ++next_it;
        if (next_it != ld.end() && *it > *next_it) {
            sorted = false;
            break;
        }
    }

    if (sorted) {
        std::cout << "Large list sorted successfully." << std::endl;
    } else {
        std::cout << "Large list sorting failed." << std::endl;
    }

    std::size_t treap_size = 100000;
    std::vector<int> vec = generateRandomSequence<int, std::vector>(treap_size, -5000000, 5000000);
    ImplicitTreap<int> treap(vec);
    comparer::CLess<int>::reset();
    int64 start, end;
    start = get_microseconds();
    PmergeMeSort(treap, comparer::CLess<int>());
    end = get_microseconds();
    std::cout << "Sorting " << treap_size << " elements in ImplicitTreap took "
        << (end - start) << " microseconds, with "
        << comparer::CLess<int>::getcnt() << " comparisons." << std::endl;
    sorted = true;
    for (std::size_t i = 1; i < treap.size(); ++i) {
        if (treap[i - 1] > treap[i]) {
            sorted = false;
            break;
        }
    }
    if (sorted) {
        std::cout << "Large ImplicitTreap sorted successfully." << std::endl;
    } else {
        std::cout << "Large ImplicitTreap sorting failed." << std::endl;
    }
    treap = ImplicitTreap<int>(vec);
    comparer::CGreater<int>::reset();
    start = get_microseconds();
    PmergeMeSort(treap, comparer::CGreater<int>());
    end = get_microseconds();
    std::cout << "Sorting " << treap_size << " elements in ImplicitTreap in descending order took "
        << (end - start) << " microseconds, with "
        << comparer::CGreater<int>::getcnt() << " comparisons." << std::endl;
    sorted = true;
    for (std::size_t i = 1; i < treap.size(); ++i) {
        if (treap[i - 1] < treap[i]) {
            sorted = false;
            break;
        }
    }
    if (sorted) {
        std::cout << "Large ImplicitTreap sorted in descending order successfully." << std::endl;
    } else {
        std::cout << "Large ImplicitTreap sorting in descending order failed." << std::endl;
    }
}
