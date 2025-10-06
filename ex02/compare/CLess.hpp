#pragma once

namespace comparer {

template<typename T>
struct CLess {
    bool operator()(const T &lhs, const T &rhs) const {
        ++_cnt;
        return lhs < rhs;
    }

    static int getcnt() {
        return _cnt;
    }
    static void reset() {
        _cnt = 0;
    }
private:
    static int _cnt;
};

template<typename T>
int CLess<T>::_cnt = 0;
}
