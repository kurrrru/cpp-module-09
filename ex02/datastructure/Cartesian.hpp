#pragma once

#include <vector>
#include <string>


class Cartesian {
 public:
    Cartesian();
    Cartesian(const std::vector<int> &arr);
    Cartesian(const Cartesian &other);
    Cartesian &operator=(const Cartesian &other);
    ~Cartesian();

    std::vector<int> sort();
    void swap(Cartesian &other);
 private:
    void build();

    int _root;
    int _n;
    std::vector<int> _parent;
    std::vector<int> _right;
    std::vector<int> _left;
    std::vector<int> _arr;
};
