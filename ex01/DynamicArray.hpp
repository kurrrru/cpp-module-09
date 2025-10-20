#pragma once

#include <memory>
#include <stdexcept>
#include <utility>

template <typename T, typename Alloc = std::allocator<T> >
class DynamicArray {
 public:
    DynamicArray()
        : _data(NULL), _size(0), _capacity(0), _allocator(Alloc()) {}

    DynamicArray(const DynamicArray& other)
        : _data(NULL), _size(0), _capacity(0), _allocator(other._allocator) {
        reserve(other._size);
        for (std::size_t i = 0; i < other._size; ++i) {
            push_back(other._data[i]);
        }
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray tmp(other);
            swap(tmp);
        }
        return *this;
    }

    ~DynamicArray() {
        clear();
        if (_data) {
            _allocator.deallocate(_data, _capacity);
        }
    }

    explicit DynamicArray(std::size_t size, const T& value = T())
        : _data(NULL), _size(0), _capacity(0), _allocator(Alloc()) {
        reserve(size);
        for (std::size_t i = 0; i < size; ++i) {
            push_back(value);
        }
    }

    void push_back(const T& value) {
        if (_size >= _capacity) {
            std::size_t new_capacity = _capacity == 0 ? 1 : _capacity * 2;
            reallocate(new_capacity);
        }
        _allocator.construct(&_data[_size], value);
        ++_size;
    }

    void pop_back() {
        if (_size > 0) {
            --_size;
            _allocator.destroy(&_data[_size]);
        }
    }

    std::size_t size() const {
        return _size;
    }

    std::size_t capacity() const {
        return _capacity;
    }

    T& operator[](std::size_t index) {
        return _data[index];
    }

    const T& operator[](std::size_t index) const {
        return _data[index];
    }

    T& at(std::size_t index) {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }

    const T& at(std::size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }

    void clear() {
        for (std::size_t i = 0; i < _size; ++i) {
            _allocator.destroy(&_data[i]);
        }
        _size = 0;
    }

    void reserve(std::size_t new_capacity) {
        if (new_capacity > _capacity) {
            reallocate(new_capacity);
        }
    }

    void swap(DynamicArray& other) {
        std::swap(_data, other._data);
        std::swap(_size, other._size);
        std::swap(_capacity, other._capacity);
        std::swap(_allocator, other._allocator);
    }

    void shrink_to_fit() {
        if (_size < _capacity) {
            reallocate(_size);
        }
    }

    void resize(std::size_t new_size, const T& value = T()) {
        if (new_size < _size) {
            for (std::size_t i = new_size; i < _size; ++i) {
                _allocator.destroy(&_data[i]);
            }
        } else if (new_size > _size) {
            reserve(new_size);
            for (std::size_t i = _size; i < new_size; ++i) {
                _allocator.construct(&_data[i], value);
            }
        }
        _size = new_size;
    }

 private:
    T* _data;
    std::size_t _size;
    std::size_t _capacity;
    Alloc _allocator;

    void reallocate(std::size_t new_capacity) {
        T* new_data = _allocator.allocate(new_capacity);
        std::size_t constructed_count = 0;
        try {
            for (std::size_t i = 0; i < _size; ++i) {
                _allocator.construct(&new_data[i], _data[i]);
                constructed_count++;
            }
        } catch (...) {
            for (std::size_t i = 0; i < constructed_count; ++i) {
                _allocator.destroy(&new_data[i]);
            }
            _allocator.deallocate(new_data, new_capacity);
            throw;
        }
        for (std::size_t i = 0; i < _size; ++i) {
            _allocator.destroy(&_data[i]);
        }
        if (_data) {
            _allocator.deallocate(_data, _capacity);
        }
        _data = new_data;
        _capacity = new_capacity;
    }
};
