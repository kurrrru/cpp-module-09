#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

struct Run {
    std::size_t start;
    std::size_t length;
};

static const std::size_t MIN_MERGE = 32;

std::size_t minRunLength(std::size_t n) {
    std::size_t r = 0;
    while (n >= MIN_MERGE) {
        r |= (n & 1u);
        n >>= 1;
    }
    return n + r;
}

template<typename Compare>
std::size_t countRunAndMakeAscending(std::vector<int> &container, std::size_t start, Compare cmp) {
    const std::size_t size = container.size();
    if (start >= size - 1) {
        return size - start;
    }

    std::size_t runEnd = start + 2;
    if (cmp(container[start + 1], container[start])) {
        while (runEnd < size && cmp(container[runEnd], container[runEnd - 1])) {
            ++runEnd;
        }
        std::reverse(container.begin() + start, container.begin() + runEnd);
    } else {
        while (runEnd < size && !cmp(container[runEnd], container[runEnd - 1])) {
            ++runEnd;
        }
    }
    return runEnd - start;
}

template<typename Compare>
void mergeAt(std::vector<int> &container, std::vector<Run> &runStack, std::size_t idx, Compare cmp) {
    Run &left = runStack[idx];
    Run &right = runStack[idx + 1];
    std::inplace_merge(container.begin() + left.start,
                    container.begin() + right.start,
                    container.begin() + right.start + right.length,
                    cmp);
    left.length += right.length;
    runStack.erase(runStack.begin() + idx + 1);
}

template<typename Compare>
void mergeCollapse(std::vector<int> &container, std::vector<Run> &runStack, Compare cmp) {
    while (runStack.size() > 1) {
        std::size_t n = runStack.size() - 1;
        std::size_t lenX = runStack[n].length;
        std::size_t lenY = runStack[n - 1].length;
        std::size_t lenZ = (n >= 2) ? runStack[n - 2].length : static_cast<std::size_t>(-1);

        if (n >= 2 && lenZ <= lenY + lenX) {
            if (lenZ < lenX) {
                mergeAt(container, runStack, n - 2, cmp);
            } else {
                mergeAt(container, runStack, n - 1, cmp);
            }
        } else if (lenY <= lenX) {
            mergeAt(container, runStack, n - 1, cmp);
        } else {
            break;
        }
    }
}

template<typename Compare>
void mergeForceCollapse(std::vector<int> &container, std::vector<Run> &runStack, Compare cmp) {
    while (runStack.size() > 1) {
        std::size_t n = runStack.size() - 1;
        if (n >= 2 && runStack[n - 2].length < runStack[n].length) {
            mergeAt(container, runStack, n - 2, cmp);
        } else {
            mergeAt(container, runStack, n - 1, cmp);
        }
    }
}

} // namespace

template<typename Compare>
void timSort(std::vector<int> &container, Compare cmp) {
    if (container.size() <= 1) {
        return;
    }

    const std::size_t minRun = minRunLength(container.size());
    std::vector<Run> runStack;
    runStack.reserve(container.size() / MIN_MERGE + 1);

    std::size_t start = 0;
    while (start < container.size()) {
        std::size_t runLen = countRunAndMakeAscending(container, start, cmp);
        if (runLen < minRun) {
            std::size_t forced = std::min(minRun, container.size() - start);
            for (std::size_t i = start + runLen; i < start + forced; ++i) {
                int key = container[i];
                std::vector<int>::iterator pos = std::lower_bound(container.begin() + start, container.begin() + i, key, cmp);
                std::rotate(pos, container.begin() + i, container.begin() + i + 1);
            }
            runLen = forced;
        }
        Run newRun;
        newRun.start = start;
        newRun.length = runLen;
        runStack.push_back(newRun);
        mergeCollapse(container, runStack, cmp);
        start += runLen;
    }

    mergeForceCollapse(container, runStack, cmp);
}