#include <ex02/benchmark/benchmark.hpp>

#include <algorithm>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <ex02/PmergeMe.hpp>
#include <ex02/clock/clock.hpp>
#include <ex02/compare/CLess.hpp>
#include <ex02/compare/CGreater.hpp>
#include <ex02/benchmark/sort_functions.hpp>
#include <ex02/utils/random_seq.hpp>
#include <toolbox/string.hpp>

namespace {
    void printResult(const std::string &sortName, std::size_t size, std::size_t maxCntCmp, double avgCntCmp, double maxTime, double avgTime) {
        std::cout << std::left << std::setw(22) << sortName
            << std::right << std::fixed << std::setprecision(2)
            << " | Size: " << std::setw(12) << size
            << " | Max Cmp: " << std::setw(12) << maxCntCmp
            << " | Avg Cmp: " << std::setw(12) << avgCntCmp
            << " | Max Time: " << std::setw(12) << maxTime << " us"
            << " | Avg Time: " << std::setw(12) << avgTime << " us"
            << std::endl;
    }

    bool isSorted(const std::vector<int> &vec) {
        for (std::size_t i = 1; i < vec.size(); ++i) {
            if (vec[i] < vec[i - 1]) {
                return false;
            }
        }
        return true;
    }
}

void benchmark() {
    const std::size_t numTrials = 20;
    const std::size_t testSize = 5000;
    std::vector<std::vector<int> > testVectors(numTrials);
    for (std::size_t i = 0; i < numTrials; ++i) {
        testVectors[i] = generateRandomSequence<int, std::vector>(testSize, 1, 10000);
    }

    std::vector<std::pair<void (*)(std::vector<int> &, comparer::CLess<int>), std::string> > sortFunctions;
    sortFunctions.push_back(std::make_pair(quickSort<comparer::CLess<int> >, "Quick Sort"));
    sortFunctions.push_back(std::make_pair(mergeSort<comparer::CLess<int> >, "Merge Sort"));
    sortFunctions.push_back(std::make_pair(heapSort<comparer::CLess<int> >, "Heap Sort"));
    sortFunctions.push_back(std::make_pair(insertionSort<comparer::CLess<int> >, "Insertion Sort"));
    sortFunctions.push_back(std::make_pair(bubbleSort<comparer::CLess<int> >, "Bubble Sort"));
    sortFunctions.push_back(std::make_pair(selectionSort<comparer::CLess<int> >, "Selection Sort"));
    sortFunctions.push_back(std::make_pair(binaryInsertionSort<comparer::CLess<int> >, "Binary Insertion Sort"));
    // sortFunctions.push_back(std::make_pair(introSort<comparer::CLess<int> >, "Intro Sort"));
    // sortFunctions.push_back(std::make_pair(timSort<comparer::CLess<int> >, "Tim Sort"));
    // sortFunctions.push_back(std::make_pair(shellSort<comparer::CLess<int> >, "Shell Sort"));
    // sortFunctions.push_back(std::make_pair(brickSort<comparer::CLess<int> >, "Brick Sort"));
    // sortFunctions.push_back(std::make_pair(combSort<comparer::CLess<int> >, "Comb Sort"));
    // sortFunctions.push_back(std::make_pair(cycleSort<comparer::CLess<int> >, "Cycle Sort"));
    // sortFunctions.push_back(std::make_pair(gnomeSort<comparer::CLess<int> >, "Gnome Sort"));
    // sortFunctions.push_back(std::make_pair(patienceSort<comparer::CLess<int> >, "Patience Sort"));
    // sortFunctions.push_back(std::make_pair(smoothSort<comparer::CLess<int> >, "Smooth Sort"));
    // sortFunctions.push_back(std::make_pair(tournamentSort<comparer::CLess<int> >, "Tournament Sort"));

    // PmergeMe sort function
    std::vector<std::vector<int> > pmergeMeTestVectors = testVectors;
    int64 maxTime = 0, totalTime = 0;
    std::size_t maxCntCmp = 0, totalCntCmp = 0;
    for (std::size_t i = 0; i < numTrials; ++i) {
        comparer::CLess<int>::reset();
        int64 start = get_microseconds();
        PmergeMeSort(pmergeMeTestVectors[i], comparer::CLess<int>());
        int64 end = get_microseconds();
        int64 elapsed = end - start;
        std::size_t cntCmp = comparer::CLess<int>::getcnt();
        if (elapsed > maxTime) {
            maxTime = elapsed;
        }
        totalTime += elapsed;
        if (cntCmp > maxCntCmp) {
            maxCntCmp = cntCmp;
        }
        totalCntCmp += cntCmp;
        if (!isSorted(pmergeMeTestVectors[i])) {
            std::cerr << "PmergeMeSort failed to sort the array." << std::endl;
            std::cout << "Original: ";
            for (std::size_t j = 0; j < testVectors[i].size(); ++j) {
                std::cout << testVectors[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    double avgCntCmp = static_cast<double>(totalCntCmp) / numTrials;
    double avgTime = static_cast<double>(totalTime) / numTrials;
    printResult("PmergeMe", testSize, maxCntCmp, avgCntCmp, maxTime, avgTime);

    // std::sort
    std::vector<std::vector<int> > stdSortTestVectors = testVectors;
    maxTime = 0, totalTime = 0, maxCntCmp = 0, totalCntCmp = 0;
    for (std::size_t i = 0; i < numTrials; ++i) {
        comparer::CLess<int>::reset();
        int64 start = get_microseconds();
        std::sort(stdSortTestVectors[i].begin(), stdSortTestVectors[i].end(), comparer::CLess<int>());
        int64 end = get_microseconds();
        int64 elapsed = end - start;
        std::size_t cntCmp = comparer::CLess<int>::getcnt();
        if (elapsed > maxTime) {
            maxTime = elapsed;
        }
        totalTime += elapsed;
        if (cntCmp > maxCntCmp) {
            maxCntCmp = cntCmp;
        }
        totalCntCmp += cntCmp;
        if (!isSorted(stdSortTestVectors[i])) {
            std::cerr << "std::sort failed to sort the array." << std::endl;
            std::cout << "Original: ";
            for (std::size_t j = 0; j < testVectors[i].size(); ++j) {
                std::cout << testVectors[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    avgCntCmp = static_cast<double>(totalCntCmp) / numTrials;
    avgTime = static_cast<double>(totalTime) / numTrials;
    printResult("std::sort", testSize, maxCntCmp, avgCntCmp, maxTime, avgTime);

    // std::stable_sort
    std::vector<std::vector<int> > stdStableSortTestVectors = testVectors;
    maxTime = 0, totalTime = 0, maxCntCmp = 0, totalCntCmp = 0;
    for (std::size_t i = 0; i < numTrials; ++i) {
        comparer::CLess<int>::reset();
        int64 start = get_microseconds();
        std::stable_sort(stdStableSortTestVectors[i].begin(), stdStableSortTestVectors[i].end(), comparer::CLess<int>());
        int64 end = get_microseconds();
        int64 elapsed = end - start;
        std::size_t cntCmp = comparer::CLess<int>::getcnt();
        if (elapsed > maxTime) {
            maxTime = elapsed;
        }
        totalTime += elapsed;
        if (cntCmp > maxCntCmp) {
            maxCntCmp = cntCmp;
        }
        totalCntCmp += cntCmp;
        if (!isSorted(stdStableSortTestVectors[i])) {
            std::cerr << "std::stable_sort failed to sort the array." << std::endl;
            std::cout << "Original: ";
            for (std::size_t j = 0; j < testVectors[i].size(); ++j) {
                std::cout << testVectors[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    avgCntCmp = static_cast<double>(totalCntCmp) / numTrials;
    avgTime = static_cast<double>(totalTime) / numTrials;
    printResult("std::stable_sort", testSize, maxCntCmp, avgCntCmp, maxTime, avgTime);

    // Other sorting algorithms
    for (std::size_t funcIdx = 0; funcIdx < sortFunctions.size(); ++funcIdx) {
        maxTime = 0, totalTime = 0, maxCntCmp = 0, totalCntCmp = 0;
        for (std::size_t i = 0; i < numTrials; ++i) {
            std::vector<int> vec = testVectors[i];
            comparer::CLess<int>::reset();
            int64 start = get_microseconds();
            sortFunctions[funcIdx].first(vec, comparer::CLess<int>());
            int64 end = get_microseconds();
            int64 elapsed = end - start;
            std::size_t cntCmp = comparer::CLess<int>::getcnt();
            if (elapsed > maxTime) {
                maxTime = elapsed;
            }
            totalTime += elapsed;
            if (cntCmp > maxCntCmp) {
                maxCntCmp = cntCmp;
            }
            totalCntCmp += cntCmp;
            if (!isSorted(vec)) {
                std::cerr << sortFunctions[funcIdx].second << " failed to sort the array." << std::endl;
                std::cout << "Original: ";
                for (std::size_t j = 0; j < testVectors[i].size(); ++j) {
                    std::cout << testVectors[i][j] << " ";
                }
                std::cout << std::endl;
            }
        }
        std::string sortName = sortFunctions[funcIdx].second;
        avgCntCmp = static_cast<double>(totalCntCmp) / numTrials;
        avgTime = static_cast<double>(totalTime) / numTrials;
        printResult(sortName, testSize, maxCntCmp, avgCntCmp, maxTime, avgTime);
    }
}
