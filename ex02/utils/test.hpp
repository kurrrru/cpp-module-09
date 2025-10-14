#pragma once

#include <vector>

#include <ex02/clock/clock.hpp>

void test();
void subject_test(int argc, char **argv);

struct TestResult {
    const char* container_name;
    std::size_t num_elements;
    int64 time_microseconds;
    bool num_comparisons_set;
    std::size_t num_comparisons;

    TestResult(const char* name, std::size_t n, int64 t)
        : container_name(name), num_elements(n), time_microseconds(t),
        num_comparisons_set(false), num_comparisons(0) {}
    TestResult(const char* name, std::size_t n, int64 t,
        std::size_t c)
        : container_name(name), num_elements(n), time_microseconds(t),
        num_comparisons_set(true), num_comparisons(c) {}
};

std::ostream& operator<<(std::ostream& out, const TestResult& result);
