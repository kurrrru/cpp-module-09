#include <iostream>
#include <stdexcept>

#include <ex02/benchmark/benchmark.hpp>
#include <ex02/utils/test.hpp>

int main(int argc, char **argv) {
    (void)argc, (void)argv, (void)subject_test;

    try {
        subject_test(argc, argv);
        test();
        benchmark();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
