#include <iostream>
#include <stdexcept>
#include <string>

#include <ex02/benchmark/benchmark.hpp>
#include <ex02/utils/test.hpp>
#include <toolbox/StepMark.hpp>

int main(int argc, char **argv) {
    (void)argc, (void)argv, (void)subject_test;

    toolbox::logger::StepMark::setLogFile("ex02.log");
    toolbox::logger::StepMark::setLevel(toolbox::logger::INFO);
    toolbox::logger::StepMark::info("ex02: application start");

    try {
        subject_test(argc, argv);
        // test();
        // benchmark();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        const std::string msg = std::string("ex02: exception: ")
            + e.what();
        toolbox::logger::StepMark::error(msg);
        return 1;
    }
    toolbox::logger::StepMark::info("ex02: application end");
    return 0;
}
