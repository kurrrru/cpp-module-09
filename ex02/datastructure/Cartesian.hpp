#pragma once

#include <algorithm>
#include <queue>
#include <utility>
#include <vector>

template<typename Compare>
struct ComparePair {
    bool operator()(const std::pair<int, int> &a, const std::pair<int, int> &b) const {
        if (Compare()(a.first, b.first)) {
            return true;
        } else if (Compare()(b.first, a.first)) {
            return false;
        } else {
            return Compare()(a.second, b.second);
        }
    }
};


template<typename Compare>
class Cartesian {
 public:
    Cartesian(): _root(-1), _n(0), _parent(), _right(), _left(), _arr() {
        build();
    }

    explicit Cartesian(const std::vector<int> &arr) : _root(-1),
        _n(arr.size()), _parent(arr.size(), -1),
        _right(arr.size(), -1), _left(arr.size(), -1), _arr(arr) {
        build();
    }

    Cartesian(const Cartesian &other)
        : _root(other._root), _n(other._n), _parent(other._parent),
        _right(other._right), _left(other._left), _arr(other._arr) {
    }

    Cartesian &operator=(const Cartesian &other) {
        Cartesian tmp(other);
        swap(tmp);
        return (*this);
    }

    ~Cartesian() {
    }

    std::vector<int> sort() const {
        if (_root == -1) {
            return std::vector<int>();
        }

        std::vector<int> sorted_arr;
        sorted_arr.reserve(_n);

        std::priority_queue<
            std::pair<int, int>,
            std::vector<std::pair<int, int> >,
            ComparePair<Compare> > pq;

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

    void swap(Cartesian &other) {
        std::swap(_root, other._root);
        std::swap(_n, other._n);
        _parent.swap(other._parent);
        _right.swap(other._right);
        _left.swap(other._left);
        _arr.swap(other._arr);
    }

 private:
    void build() {
        std::vector<int> st;
        for (int i = 0; i < _n; i++) {
            if (_root == -1) {
                _root = i;
                st.push_back(i);
                continue;
            }
            while (!st.empty() && Compare()(_arr[st.back()], _arr[i])) {
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

    int _root;
    int _n;
    std::vector<int> _parent;
    std::vector<int> _right;
    std::vector<int> _left;
    std::vector<int> _arr;
};
