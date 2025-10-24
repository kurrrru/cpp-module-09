#include <deque>
#include <iostream>
#include <iomanip>
// #include <list>
#include <string>
#include <vector>

#include <ex02/PmergeMe.hpp>
#include <ex02/clock/clock.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/datastructure/ImplicitTreap.hpp>
#include <ex02/utils/random_seq.hpp>
#include <ex02/utils/test.hpp>
#include <ex02/utils/utils.hpp>
#include <toolbox/string.hpp>
#include <toolbox/StepMark.hpp>

namespace {
std::size_t longestIntegerWidth(const std::vector<int>& vec) {
    int max_val = *std::max_element(vec.begin(), vec.end());
    return toolbox::to_string(max_val).size();
}

std::vector<int> parseArguments(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Error: At least one positive "
            "integer argument is required." << std::endl;
        toolbox::logger::StepMark::error(
            "subject_test: no arguments provided");
        exit(1);
    }
    std::vector<int> vec(argc - 1);
    try {
        for (int i = 1; i < argc; ++i) {
            int num = toolbox::stoi(argv[i]);
            if (num <= 0) {
                std::cerr << "Error: All arguments must be "
                    "positive integers." << std::endl;
                toolbox::logger::StepMark::error(
                    "subject_test: argument not positive");
                exit(1);
            }
            vec[i - 1] = num;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Conversion failed: " << e.what() << std::endl;
        const std::string msg = std::string(
            "subject_test: conversion failed: ") + e.what();
        toolbox::logger::StepMark::error(msg);
        exit(1);
    }
    const std::string msg = std::string("subject_test: parsed ")
        + toolbox::to_string(vec.size()) + " values";
    toolbox::logger::StepMark::info(msg);
    return vec;
}

}  // namespace

void subject_test(int argc, char **argv) {
    std::vector<int> input = parseArguments(argc, argv);
    const std::string start_msg = std::string("subject_test: start size=")
        + toolbox::to_string(input.size());
    toolbox::logger::StepMark::info(start_msg);
    const bool display_comparison = false;
    const std::size_t width = longestIntegerWidth(input);
    std::vector<TestResult> results;
    std::vector<int> sortedInput;
    {
        std::vector<int> vec = input;
        int64 start, end;
        comparer::CLess<int>::reset();
        start = get_microseconds();
        PmergeMeSort(vec, comparer::CLess<int>());
        end = get_microseconds();
        bool vec_sorted = true;
        for (size_t i = 1; i < vec.size(); ++i) {
            if (vec[i - 1] > vec[i]) {
                vec_sorted = false;
                break;
            }
        }
        if (!vec_sorted) {
            std::cerr << "Error: Vector sorting failed." << std::endl;
            toolbox::logger::StepMark::error(
                "subject_test: std::vector validation failed");
            exit(1);
        }
        if (display_comparison) {
            results.push_back(TestResult("std::vector", vec.size(),
                end - start, comparer::CLess<int>::getcnt()));
        } else {
            results.push_back(TestResult("std::vector", vec.size(),
                end - start));
        }
        sortedInput = vec;
    }
    {
        std::deque<int> deq(input.begin(), input.end());
        int64 start, end;
        comparer::CLess<int>::reset();
        start = get_microseconds();
        PmergeMeSort(deq, comparer::CLess<int>());
        end = get_microseconds();
        bool deq_sorted = true;
        for (size_t i = 1; i < deq.size(); ++i) {
            if (deq[i - 1] > deq[i]) {
                deq_sorted = false;
                break;
            }
        }
        if (!deq_sorted) {
            std::cerr << "Error: Deque sorting failed." << std::endl;
            toolbox::logger::StepMark::error(
                "subject_test: std::deque validation failed");
            exit(1);
        }
        if (display_comparison) {
            results.push_back(TestResult("std::deque", deq.size(),
                end - start, comparer::CLess<int>::getcnt()));
        } else {
            results.push_back(TestResult("std::deque", deq.size(),
                end - start));
        }
    }
    // {
    //     std::list<int> lst(input.begin(), input.end());
    //     int64 start, end;
    //     comparer::CLess<int>::reset();
    //     start = get_microseconds();
    //     PmergeMeSort(lst, comparer::CLess<int>());
    //     end = get_microseconds();
    //     bool lst_sorted = true;
    //     for (std::list<int>::iterator it = lst.begin(),
    //         next_it = lst.begin(); it != lst.end(); ++it) {
    //         next_it = it;
    //         ++next_it;
    //         if (next_it != lst.end() && *it > *next_it) {
    //             lst_sorted = false;
    //             break;
    //         }
    //     }
    //     if (!lst_sorted) {
    //         std::cerr << "Error: List sorting failed." << std::endl;
    //         toolbox::logger::StepMark::error(
    //             "subject_test: std::list validation failed");
    //         exit(1);
    //     }
    //     if (display_comparison) {
    //         results.push_back(TestResult("std::list", lst.size(),
    //             end - start, comparer::CLess<int>::getcnt()));
    //     } else {
    //         results.push_back(TestResult("std::list", lst.size(),
    //             end - start));
    //     }
    // }
    // {
    //     ImplicitTreap<int> treap(input);
    //     int64 start, end;
    //     comparer::CLess<int>::reset();
    //     start = get_microseconds();
    //     PmergeMeSort(treap, comparer::CLess<int>());
    //     end = get_microseconds();
    //     bool treap_sorted = true;
    //     for (size_t i = 1; i < treap.size(); ++i) {
    //         if (treap[i - 1] > treap[i]) {
    //             treap_sorted = false;
    //             break;
    //         }
    //     }
    //     if (!treap_sorted) {
    //         std::cerr << "Error: Treap sorting failed." << std::endl;
    //         toolbox::logger::StepMark::error(
    //             "subject_test: ImplicitTreap validation failed");
    //         exit(1);
    //     }
    //     if (display_comparison) {
    //         results.push_back(TestResult("ImplicitTreap", treap.size(),
    //         end - start, comparer::CLess<int>::getcnt()));
    //     } else {
    //         results.push_back(TestResult("ImplicitTreap", treap.size(),
    //         end - start));
    //     }
    // }
    std::cout << "Before: ";
    printContainer(input, width);
    std::cout << "After:  ";
    printContainer(sortedInput, width);
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << results[i] << std::endl;
    }
    toolbox::logger::StepMark::info("subject_test: completed");
}

std::ostream& operator<<(std::ostream& out, const TestResult& result) {
    out << "Time to process a range of "
        << std::right << std::setw(7) << result.num_elements
        << " elements with "
        << std::left << std::setw(16) << result.container_name
        << ": "
        << std::right << std::setw(10) << result.time_microseconds
        << " us";
    if (result.num_comparisons_set) {
        out << " ("
            << std::right << std::setw(10) << result.num_comparisons
            << " comparisons)";
    }
    return out;
}
