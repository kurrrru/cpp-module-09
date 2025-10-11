#include "Cartesian.hpp"
#include <algorithm>
#include <iostream>
#include <queue>

Cartesian::Cartesian()
    : _root(-1), _n(0), _parent(), _right(), _left(), _arr() {
    build();
}

Cartesian::Cartesian(const Cartesian &other)
    : _root(other._root), _n(other._n), _parent(other._parent),
    _right(other._right), _left(other._left), _arr(other._arr) {
}

Cartesian &Cartesian::operator=(const Cartesian &other) {
    Cartesian tmp(other);
    swap(tmp);
    return (*this);
}

Cartesian::~Cartesian() {
}

Cartesian::Cartesian(const std::vector<int> &arr)
    : _root(-1), _n(arr.size()), _parent(arr.size(), -1),
    _right(arr.size(), -1), _left(arr.size(), -1), _arr(arr) {
    build();
}

void Cartesian::swap(Cartesian &other) {
    std::swap(_root, other._root);
    std::swap(_n, other._n);
    _parent.swap(other._parent);
    _right.swap(other._right);
    _left.swap(other._left);
    _arr.swap(other._arr);
}

void Cartesian::build() {
    std::vector<int> st;
    for (int i = 0; i < _n; i++) {
        if (_root == -1) {
            _root = i;
            st.push_back(i);
            continue;
        }
        while (!st.empty() && std::less<int>()(_arr[st.back()], _arr[i])) { // ここはcmpで比較するように変更する
            st.pop_back();
        }
        if (st.empty()) {
            _left[i] = _root;
            _parent[_root] = i;
            _root = i;
            st.push_back(i);
        } else {
            _left[i] = _right[st.back()];
            _parent[st.back()] = i;
            _right[st.back()] = i;
            _parent[i] = st.back();
            st.push_back(i);
        }
    }
}

std::vector<int> Cartesian::sort() {
    if (_root == -1) {
        return std::vector<int>();
    }

    std::vector<int> sorted_arr;
    sorted_arr.reserve(_n);

    std::priority_queue<
        std::pair<int, int>,
        std::vector<std::pair<int, int> >,
        std::less<std::pair<int, int> > > pq; // cmp

    pq.push(std::make_pair(_arr[_root], _root));

    while (!pq.empty()) {
        std::pair<int, int> top = pq.top();
        pq.pop();
        int current_val = top.first;
        int current_idx = top.second;
        sorted_arr.push_back(current_val);
        if (_left[current_idx] != -1) {
            int left_child_idx = _left[current_idx];
            pq.push(std::make_pair(_arr[left_child_idx], left_child_idx));
        }
        if (_right[current_idx] != -1) {
            int right_child_idx = _right[current_idx];
            pq.push(std::make_pair(_arr[right_child_idx], right_child_idx));
        }
    }
    std::reverse(sorted_arr.begin(), sorted_arr.end());
    return sorted_arr;
}


int main(void) {
    std::vector<int> arr;
    arr.push_back(3);
    arr.push_back(2);
    arr.push_back(1);
    arr.push_back(6);
    arr.push_back(0);
    arr.push_back(5);
    Cartesian c(arr);
    std::vector<int> sorted = c.sort();
    for (size_t i = 0; i < sorted.size(); i++) {
        std::cout << sorted[i] << " ";
    }
    std::cout << std::endl;
}