#include <ex02/detail/utils.hpp>

#include <utility>

// -> utils.{hpp,cpp}
void nextJacobsthal(std::pair<std::size_t, std::size_t> &j) {
    std::swap(j.first, j.second);
    j.first = j.first * 2 + j.second;
}
