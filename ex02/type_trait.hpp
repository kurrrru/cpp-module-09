#pragma once

#include <deque>
// #include <list>
#include <vector>

struct true_type {
    static const bool value = true;
};

struct false_type {
    static const bool value = false;
};

template<typename T>
struct is_vector : false_type {};

template<typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc> > : true_type {};

template<typename T>
struct is_deque : false_type {};

template<typename T, typename Alloc>
struct is_deque<std::deque<T, Alloc> > : true_type {};

// template<typename T>
// struct is_list : false_type {};

// template<typename T, typename Alloc>
// struct is_list<std::list<T, Alloc> > : true_type {};
