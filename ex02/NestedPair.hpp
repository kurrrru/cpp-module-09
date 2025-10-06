#pragma once

template<typename T, typename Compare>
class NestedPair {
 public:
    NestedPair() : _bigger(T()), _smaller(NULL) {}
    NestedPair(const NestedPair &other) : _bigger(other._bigger) {
        if (other._smaller) {
            _smaller = new NestedPair(*other._smaller);
        } else {
            _smaller = NULL;
        }
    }
    NestedPair &operator=(const NestedPair &other) {
        NestedPair tmp(other);
        swap(tmp);
        return *this;
    }
    ~NestedPair() {
        delete _smaller;
    }

    NestedPair(T value1, T value2) {
        if (Compare()(value1, value2)) {
            _bigger = value1;
            _smaller = new NestedPair(value2);
        } else {
            _bigger = value2;
            _smaller = new NestedPair(value1);
        }
    }
    NestedPair(const NestedPair &p1, const NestedPair &p2) {
        if (Compare()(p1._bigger, p2._bigger)) {
            _bigger = p1._bigger;
            _smaller = new NestedPair(p1._smaller ? *p1._smaller : NestedPair(), p2);
        } else {
            _bigger = p2._bigger;
            _smaller = new NestedPair(p2._smaller ? *p2._smaller : NestedPair(), p1);
        }
    }
    explicit NestedPair(T value) : _bigger(value), _smaller(NULL) {}

    void swap(NestedPair &other) {
        std::swap(_bigger, other._bigger);
        std::swap(_smaller, other._smaller);
    }

    int getBigger() const {
        if (_smaller) {
            return _bigger + _smaller->getBigger();
        } else {
            return _bigger;
        }
    }

    bool isSmallerNull() const {
        return _smaller == NULL;
    }

    NestedPair *extractSmaller() {
        NestedPair *temp = _smaller;
        _smaller = NULL;
        return temp;
    }

    bool operator==(const NestedPair &other) const {
        if (_bigger != other._bigger) {
            return false;
        }
        if (_smaller == NULL && other._smaller == NULL) {
            return true;
        }
        if (_smaller == NULL || other._smaller == NULL) {
            return false;
        }
        return *_smaller == *other._smaller;
    }
    bool operator!=(const NestedPair &other) const {
        return !(*this == other);
    }
    bool operator<(const NestedPair &other) const {
        return Compare()(_bigger, other._bigger);
    }
    bool operator<=(const NestedPair &other) const {
        return *this < other || *this == other;
    }
    bool operator>(const NestedPair &other) const {
        return !(*this <= other);
    }
    bool operator>=(const NestedPair &other) const {
        return !(*this < other);
    }

 private:
    T _bigger;
    NestedPair *_smaller;
};
