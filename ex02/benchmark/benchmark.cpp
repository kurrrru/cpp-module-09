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

namespace {
    void printResult(const std::string &sortName, std::size_t size, std::size_t maxCntCmp, double avgCntCmp, double maxTime, double avgTime) {
        std::cout << std::left << std::setw(20) << sortName
            << " | Size: " << std::setw(10) << size
            << " | Max Cmp: " << std::setw(10) << maxCntCmp
            << " | Avg Cmp: " << std::setw(10) << avgCntCmp
            << " | Max Time: " << std::setw(10) << maxTime << " us"
            << " | Avg Time: " << std::setw(10) << avgTime << " us"
            << std::endl;
    }
}

void benchmark() {
    const std::size_t numTrials = 10;
    const std::size_t testSize = 500;
    std::vector<std::vector<int> > testVectors(numTrials);
    for (std::size_t i = 0; i < numTrials; ++i) {
        testVectors[i] = generateRandomSequence<int, std::vector>(testSize, 1, 10000);
    }

    std::vector<void (*)(std::vector<int> &, comparer::CLess<int>)> sortFunctions;
    // sortFunctions.push_back(quickSort<comparer::CLess<int> >);
    sortFunctions.push_back(mergeSort<comparer::CLess<int> >);
    // sortFunctions.push_back(heapSort<comparer::CLess<int> >);
    // sortFunctions.push_back(insertionSort<comparer::CLess<int> >);
    // sortFunctions.push_back(bubbleSort<comparer::CLess<int> >);
    // sortFunctions.push_back(selectionSort<comparer::CLess<int> >);
    // sortFunctions.push_back(binaryInsertionSort<comparer::CLess<int> >);
    // sortFunctions.push_back(timSort<comparer::CLess<int> >);
    // sortFunctions.push_back(shellSort<comparer::CLess<int> >);
    // sortFunctions.push_back(introSort<comparer::CLess<int> >);
    // sortFunctions.push_back(brickSort<comparer::CLess<int> >);
    // sortFunctions.push_back(combSort<comparer::CLess<int> >);
    // sortFunctions.push_back(cycleSort<comparer::CLess<int> >);
    // sortFunctions.push_back(gnomeSort<comparer::CLess<int> >);
    // sortFunctions.push_back(patienceSort<comparer::CLess<int> >);
    // sortFunctions.push_back(smoothSort<comparer::CLess<int> >);
    // sortFunctions.push_back(tournamentSort<comparer::CLess<int> >);

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
    }
    printResult("PmergeMe", testSize, maxCntCmp, static_cast<double>(totalCntCmp) / numTrials, maxTime, static_cast<double>(totalTime) / numTrials);

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
    }
    printResult("std::sort", testSize, maxCntCmp, static_cast<double>(totalCntCmp) / numTrials, maxTime, static_cast<double>(totalTime) / numTrials);

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
    }
    printResult("std::stable_sort", testSize, maxCntCmp, static_cast<double>(totalCntCmp) / numTrials, maxTime, static_cast<double>(totalTime) / numTrials);
}
