#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

/**
 * @brief Monoid modeling range addition queries with lazy propagation support.
 */
struct monoid_range_add_range_sum {
    /**
     * @brief Combine two query results using addition.
     * @param a Prefix sum of the left operand.
     * @param b Prefix sum of the right operand.
     * @return The aggregated range sum.
     */
    static int query_op(int a, int b) {
        return (a + b);
    }
    /**
     * @brief Identity element of the range-sum monoid used for queries.
     * @return Additive identity (0), which leaves any prefix sum unchanged.
     */
    static int query_id() {
        return (0);
    }
    /**
     * @brief Combine two lazy update values.
     * @param a Existing lazy value.
     * @param b Increment to apply.
     * @return Accumulated lazy value after composition.
     */
    static int update_op(int a, int b) {
        return (a + b);
    }
    /**
     * @brief Identity element of the range-update monoid.
     * @return Additive identity (0), meaning no increment is pending.
     */
    static int update_id() {
        return (0);
    }
    /**
     * @brief Apply a lazy update to an aggregated value.
     * @param a Current aggregated sum.
     * @param b Lazy increment to add.
     * @param len Number of elements represented by the aggregate.
     * @return Updated aggregate after applying the lazy increment.
     */
    static int apply(int a, int b, int len) {
        return (b == update_id() ? a : a + b * len);
    }
};

/**
 * @brief Monoid representing point assignment semantics for pairs and custom values.
 *
 * This monoid overwrites values during queries and updates, mirroring direct assignment
 * operations that discard the prior state.
 */
template<typename Value>
struct monoid_point_assign {
    typedef Value value_type;
    /**
     * @brief Combine two query results by returning the most recent assignment.
     * @param a Ignored previous value.
     * @param b New value to propagate forward.
     * @return The newly assigned value @p b.
     */
    static value_type query_op(const value_type &, const value_type &b) {
        return b;
    }
    /**
     * @brief Identity element of the point-assign query monoid.
     * @return Default-constructed value acting as the neutral element.
     */
    static value_type query_id() {
        return value_type();
    }
    /**
     * @brief Compose two updates, keeping the last assignment.
     * @param a Ignored existing pending assignment.
     * @param b Replacement value that overrides previous updates.
     * @return The overriding assignment @p b.
     */
    static value_type update_op(const value_type &, const value_type &b) {
        return b;
    }
    /**
     * @brief Identity element of the point-assign update monoid.
     * @return Default-constructed value signifying no pending overwrite.
     */
    static value_type update_id() {
        return value_type();
    }
    static value_type apply(const value_type &a, const value_type &b, int) {
        return (b == update_id() ? a : b);
    }
};

/**
 * @brief Primary traits mapping values to their default monoid implementations.
 *
 * For scalar values the range-add & range-sum monoid is used by default.
 */
template<typename T>
struct monoid_traits {
    typedef monoid_range_add_range_sum type;
};

/**
 * @brief Traits specialization selecting point-assign semantics for pair values.
 */
template<typename First, typename Second>
struct monoid_traits<std::pair<First, Second> > {
    typedef monoid_point_assign<std::pair<First, Second> > type;
};

/**
 * @brief Sequence container implemented as an implicit treap with lazy propagation.
 *
 * The container stores values in positional order while supporting logarithmic
 * insertion, erasure, range updates, and range queries. Internally it relies on
 * a randomized binary tree augmented with lazy propagation flags for range
 * addition and reversal.
 *
 * @tparam T Value type stored in the container.
 * @tparam Allocator Allocator used to manage node storage.
 */
template<typename T, class Allocator = std::allocator<T> >
class ImplicitTreap {
 private:
    struct node;
 public:
    class value_proxy;
     class iterator;
    typedef T                                        value_type;
    typedef Allocator                                allocator_type;
    typedef typename Allocator::reference            reference;
    typedef typename Allocator::const_reference      const_reference;
    typedef typename Allocator::pointer              pointer;
    typedef typename Allocator::const_pointer        const_pointer;
    typedef std::size_t                              size_type;
    typedef std::ptrdiff_t                           difference_type;

    typedef typename monoid_traits<value_type>::type operations;

    /**
     * @brief Construct an empty treap with an optional allocator.
     * @param alloc Allocator instance copied for node management.
     */
    ImplicitTreap(const Allocator &alloc = Allocator()) 
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
    }
    /**
     * @brief Destroy the treap and release all allocated nodes.
     */
    ~ImplicitTreap() {
        clear();
    }

    /**
     * @brief Construct the treap by inserting values from a vector.
     * @param vec Source vector whose values are inserted in order.
     * @param alloc Allocator instance copied for node management.
     */
    ImplicitTreap(std::vector<value_type> &vec, const Allocator &alloc = Allocator())
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
        for (size_type i = 0; i < vec.size(); i++) {
            insert(i, vec[i]);
        }
    }
    /**
     * @brief Construct the treap with a repeated value.
     * @param size Number of elements to insert.
     * @param val Value assigned to each inserted element.
     * @param alloc Allocator instance copied for node management.
     */
    ImplicitTreap(size_type size, value_type val, const Allocator &alloc = Allocator())
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
        for (size_type i = 0; i < size; i++) {
            insert(i, val);
        }
    }

    /**
     * @brief Copy-construct the treap.
     * @param other Treap whose contents are duplicated.
     */
    ImplicitTreap(const ImplicitTreap &other)
        : _root(NULL), _node_alloc(other._node_alloc), _rnd(other._rnd) {
        _root = clone_subtree(other._root, NULL);
    }

    /**
     * @brief Copy-assign from another treap.
     * @param other Treap whose contents are copied.
     * @return Reference to this container.
     */
    ImplicitTreap &operator=(const ImplicitTreap &other) {
        if (this != &other) {
            ImplicitTreap tmp(other);
            swap(tmp);
        }
        return (*this);
    }

    /**
     * @brief Exchange contents with another treap.
     * @param other Treap to swap state with.
     */
    void swap(ImplicitTreap &other) {
        std::swap(_root, other._root);
        std::swap(_node_alloc, other._node_alloc);
        std::swap(_rnd, other._rnd);
    }

    /**
     * @brief Query the aggregated value on a half-open interval.
     * @param l Inclusive range start.
     * @param r Exclusive range end.
     * @return Range sum over [l, r).
     */
    value_type query(size_type l, size_type r) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        return (query(_root, l, r));
    }

    /**
     * @brief Find the first position where prefix sum reaches at least a value.
     * @param l Inclusive search lower bound.
     * @param r Exclusive search upper bound.
     * @param val Threshold added on top of prefix sum at l.
     * @return Index in [l, r] where the threshold is met or r if not found.
     */
    size_type query_lower_bound(size_type l, size_type r, value_type val) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        if (l >= r) {
            return (r);
        }
        if (val <= operations::query_id()) {
            return (l);
        }
        value_type prefix_l = prefix_sum(_root, l);
        value_type prefix_r = prefix_sum(_root, r);
        value_type target = operations::query_op(prefix_l, val);
        if (target > prefix_r) {
            return (r);
        }
        size_type idx = prefix_bound(_root, target, false);
        size_type limit = r;
        size_type total = cnt(_root);
        if (idx > limit || idx > total) {
            return (r);
        }
        if (idx < l) {
            return (l);
        }
        return (idx);
    }

    /**
     * @brief Find the first position where prefix sum becomes greater than a value.
     * @param l Inclusive search lower bound.
     * @param r Exclusive search upper bound.
     * @param val Threshold added on top of prefix sum at l.
     * @return Index in [l, r] just after exceeding the threshold or r otherwise.
     */
    size_type query_upper_bound(size_type l, size_type r, value_type val) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        if (l >= r) {
            return (r);
        }
        if (val < operations::query_id()) {
            return (l);
        }
        value_type prefix_l = prefix_sum(_root, l);
        value_type prefix_r = prefix_sum(_root, r);
        value_type target = operations::query_op(prefix_l, val);
        if (prefix_r <= target) {
            return (r);
        }
        size_type idx = prefix_bound(_root, target, true);
        size_type total = cnt(_root);
        if (idx > r || idx > total) {
            return (r);
        }
        if (idx < l) {
            return (l);
        }
        return (idx);
    }

    /**
     * @brief Apply a point update to an element.
     * @param pos Index of the element to modify.
     * @param val Lazy increment applied to the position.
     */
    void update(size_type pos, value_type val) {
        size_type current_size = size();
        assert(pos < current_size);
        update(_root, pos, pos + 1, val);
    }

    /**
     * @brief Apply a range update on a half-open interval.
     * @param l Inclusive range start.
     * @param r Exclusive range end.
     * @param val Lazy increment applied across the interval.
     */
    void update(size_type l, size_type r, value_type val) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        update(_root, l, r, val);
    }

    /**
     * @brief Access element with proxy allowing assignment.
     * @param pos Position of the element to access.
     * @return Proxy reference that materializes lazy state on demand.
     */
    value_proxy operator[](size_type pos) {
        size_type current_size = size();
        assert(pos < current_size);
        node* target_node = find_node_by_index(pos);
        assert(target_node);
        return value_proxy(this, target_node);
    }

    /**
     * @brief Read-only element access with lazy evaluation.
     * @param pos Position of the element to access.
     * @return Value stored at the position.
     */
    value_type operator[](size_type pos) const {
        size_type current_size = size();
        assert(pos < current_size);
        return value_at_const(_root, pos, operations::update_id(), false);
    }

    /**
     * @brief Insert a value at a specific index.
     * @param pos Position where the value is inserted.
     * @param val Value to insert.
     */
    void insert(size_type pos, value_type val) {
        node* item = _node_alloc.allocate(1);
        try {
            _node_alloc.construct(item, node(val, _rnd.random()));
        } catch (...) {
            _node_alloc.deallocate(item, 1);
            throw;
        }
        insert(_root, pos, item);
    }

    void push_back(const value_type &val) {
        insert(size(), val);
    }

    void push_front(const value_type &val) {
        insert(0, val);
    }

    value_proxy front() {
        size_type current_size = size();
        assert(current_size > 0);
        return (*this)[0];
    }

    value_type front() const {
        size_type current_size = size();
        assert(current_size > 0);
        return (*this)[0];
    }

    value_proxy back() {
        size_type current_size = size();
        assert(current_size > 0);
        return (*this)[current_size - 1];
    }

    value_type back() const {
        size_type current_size = size();
        assert(current_size > 0);
        return (*this)[current_size - 1];
    }

    iterator insert(iterator position, const value_type &val) {
    size_type index = node_index(position.base_node());
        insert(index, val);
        node *inserted = find_node_by_index(index);
        return iterator(this, inserted);
    }

    /**
     * @brief Erase the element at an index.
     * @param pos Position of the element to remove.
     */
    void erase(size_type pos) {
        size_type current_size = size();
        assert(pos < current_size);
        erase(_root, pos);
    }

    /**
     * @brief Retrieve the current number of elements.
     * @return Container size.
     */
    size_type size() const {
        if (!_root) {
            return 0;
        }
        return cnt(_root);
    }

    /**
     * @brief Find the first index whose value is not less than the target.
     * @param l Unused left boundary retained for interface symmetry.
     * @param r Unused right boundary retained for interface symmetry.
     * @param val Value to compare against stored elements.
     * @return Number of elements strictly less than @p val.
     */
    int lower_bound(size_type l, size_type r, value_type val) {
        node *left, *right, *mid;
        split(_root, l, left, right);
        split(right, r - l, mid, right);
        int result = lower_bound(mid, val);
        merge(right, mid, right);
        merge(_root, left, right);
        return (l + result);
    }

    /**
     * @brief Find the first index whose value is greater than the target.
     * @param l Unused left boundary retained for interface symmetry.
     * @param r Unused right boundary retained for interface symmetry.
     * @param val Value to compare against stored elements.
     * @return Number of elements less than or equal to @p val.
     */
    int upper_bound(size_type l, size_type r, value_type val) {
        node *left, *right, *mid;
        split(_root, l, left, right);
        split(right, r - l, mid, right);
        int result = upper_bound(mid, val);
        merge(right, mid, right);
        merge(_root, left, right);
        return (l + result);
    }

    /**
     * @brief Reverse values on the half-open interval [l, r).
     * @param l Inclusive range start.
     * @param r Exclusive range end.
     */
    void reverse(size_type l, size_type r) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        reverse(_root, l, r);
    }

    /**
     * @brief Rotate the interval [l, r) so that index m moves to the front.
     * @param l Inclusive range start.
     * @param m Middle offset that becomes the new front.
     * @param r Exclusive range end.
     */
    void rotate(size_type l, size_type m, size_type r) {
        size_type current_size = size();
        assert(l <= m && m <= r && r <= current_size);
        rotate(_root, l, m, r);
    }

    /**
     * @brief Remove all elements from the treap.
     */
    void clear() {
        clear(_root);
        _root = NULL;
    }

    /**
     * @brief Immutable bidirectional iterator over the treap.
     */
    class const_iterator: public std::iterator<std::bidirectional_iterator_tag, value_type> {
     public:
        /**
         * @brief Construct an end iterator.
         */
        const_iterator(): _treap(NULL), _index(0), _cache() {}
        /**
         * @brief Construct an iterator positioned at an index.
         * @param treap Container being traversed.
         * @param index Zero-based position within the treap.
         */
        const_iterator(const ImplicitTreap *treap, size_type index)
            : _treap(treap), _index(index), _cache() {}
        /**
         * @brief Copy-construct the iterator.
         * @param other Iterator to copy state from.
         */
        const_iterator(const const_iterator &other)
            : _treap(other._treap), _index(other._index), _cache(other._cache) {}
        /**
         * @brief Copy-assign from another iterator.
         * @param other Iterator to copy state from.
         * @return Reference to this iterator.
         */
        const_iterator &operator=(const const_iterator &other) {
            if (this != &other) {
                _treap = other._treap;
                _index = other._index;
                _cache = other._cache;
            }
            return (*this);
        }
        /**
         * @brief Destroy the iterator.
         */
        ~const_iterator() {}

        /**
         * @brief Dereference to obtain the value at the current position.
         * @return Value stored at the iterator's position.
         */
        value_type operator*() const {
            assert(_treap);
            assert(_index < _treap->size());
            _cache = (*_treap)[_index];
            return _cache;
        }
        /**
         * @brief Arrow operator returning a pointer to the cached value.
         * @return Pointer to the cached value.
         */
        const value_type *operator->() const {
            assert(_treap);
            assert(_index < _treap->size());
            _cache = (*_treap)[_index];
            return &_cache;
        }

        /**
         * @brief Advance to the next position.
         * @return Reference to this iterator.
         */
        const_iterator &operator++() {
            assert(_treap);
            assert(_index < _treap->size());
            ++_index;
            return (*this);
        }
        /**
         * @brief Advance to the next position, returning the previous state.
         * @return Iterator prior to the increment.
         */
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return (tmp);
        }
        /**
         * @brief Move to the previous position.
         * @return Reference to this iterator.
         */
        const_iterator &operator--() {
            assert(_treap);
            assert(_index > 0);
            --_index;
            return (*this);
        }
        /**
         * @brief Move to the previous position, returning the prior state.
         * @return Iterator prior to the decrement.
         */
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return (tmp);
        }
        /**
         * @brief Compare iterators for equality.
         * @param other Iterator to compare against.
         * @return True if both iterators reference the same container and index.
         */
        bool operator==(const const_iterator &other) const {
            return (_treap == other._treap && _index == other._index);
        }
        /**
         * @brief Compare iterators for inequality.
         * @param other Iterator to compare against.
         * @return True if either container or index differs.
         */
        bool operator!=(const const_iterator &other) const {
            return (!(*this == other));
        }

        /**
         * @brief Advance by an offset.
         * @param n Signed distance to move forward.
         * @return Reference to this iterator.
         */
        const_iterator &operator+=(difference_type n) {
            assert(_treap);
            difference_type target = static_cast<difference_type>(_index) + n;
            assert(target >= 0 && target <= static_cast<difference_type>(_treap->size()));
            _index = static_cast<size_type>(target);
            return (*this);
        }

        /**
         * @brief Move backward by an offset.
         * @param n Signed distance to move backward.
         * @return Reference to this iterator.
         */
        const_iterator &operator-=(difference_type n) {
            return (*this += -n);
        }

        /**
         * @brief Create a new iterator advanced by an offset.
         * @param n Signed distance to move forward.
         * @return Iterator positioned @p n steps ahead.
         */
        const_iterator operator+(difference_type n) const {
            const_iterator tmp(*this);
            tmp += n;
            return tmp;
        }

        /**
         * @brief Create a new iterator moved backward by an offset.
         * @param n Signed distance to move backward.
         * @return Iterator positioned @p n steps behind.
         */
        const_iterator operator-(difference_type n) const {
            const_iterator tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief Compute the distance between two iterators.
         * @param other Iterator to subtract.
         * @return Signed distance between iterators.
         */
        difference_type operator-(const const_iterator &other) const {
            assert(_treap == other._treap);
            return static_cast<difference_type>(_index) - static_cast<difference_type>(other._index);
        }

        /**
         * @brief Lexicographically compare iterator positions.
         * @param other Iterator to compare.
         * @return True if this iterator precedes @p other.
         */
        bool operator<(const const_iterator &other) const {
            assert(_treap == other._treap);
            return _index < other._index;
        }

        /**
         * @brief Lexicographically compare iterator positions.
         * @param other Iterator to compare.
         * @return True if this iterator follows @p other.
         */
        bool operator>(const const_iterator &other) const {
            return (other < *this);
        }

        /**
         * @brief Lexicographically compare iterator positions for <=.
         * @param other Iterator to compare.
         * @return True if this iterator does not follow @p other.
         */
        bool operator<=(const const_iterator &other) const {
            return !(*this > other);
        }

        /**
         * @brief Lexicographically compare iterator positions for >=.
         * @param other Iterator to compare.
         * @return True if this iterator does not precede @p other.
         */
        bool operator>=(const const_iterator &other) const {
            return !(*this < other);
        }

     private:
        const ImplicitTreap *_treap;
        size_type _index;
        mutable value_type _cache;
    };

    /**
     * @brief Proxy object providing read-write access with lazy materialization.
     */
    class value_proxy {
     public:
        /**
         * @brief Construct a proxy bound to a node.
         * @param treap Owning container.
         * @param n Node being referenced.
         */
        value_proxy(ImplicitTreap *treap, node *n)
            : _treap(treap), _node(n) {}
        /**
         * @brief Copy-construct the proxy.
         * @param other Proxy to duplicate.
         */
        value_proxy(const value_proxy &other)
            : _treap(other._treap), _node(other._node) {}
        /**
         * @brief Assign a value to the referenced node.
         * @param val New value to store.
         * @return Reference to this proxy.
         */
        value_proxy &operator=(const value_type &val) {
            assert(_treap);
            assert(_node);
            _treap->assign_node_value(_node, val);
            return (*this);
        }
        /**
         * @brief Assign from another proxy.
         * @param other Proxy whose value is read.
         * @return Reference to this proxy.
         */
        value_proxy &operator=(const value_proxy &other) {
            return (*this = static_cast<value_type>(other));
        }
        /**
         * @brief Implicit conversion to the referenced value.
         * @return Materialized value stored in the node.
         */
        operator value_type() const {
            assert(_treap);
            assert(_node);
            return (_treap->node_value(_node));
        }
        /**
         * @brief Arrow operator exposing a mutable pointer.
         * @return Pointer to the node's value after lazy propagation.
         */
        value_type *operator->() {
            assert(_treap);
            assert(_node);
            _treap->materialize_node(_node);
            return (&_node->_value);
        }
        /**
         * @brief Arrow operator exposing a const pointer.
         * @return Pointer to the node's value after lazy propagation.
         */
        const value_type *operator->() const {
            assert(_treap);
            assert(_node);
            _treap->materialize_node(_node);
            return (&_node->_value);
        }

     private:
        ImplicitTreap *_treap;
        node *_node;
    };

    /**
     * @brief Mutable bidirectional iterator over the treap.
     */
    class iterator {
     public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename ImplicitTreap::value_type value_type;
        typedef typename ImplicitTreap::difference_type difference_type;
        typedef value_proxy reference;
        typedef value_type* pointer;

        /**
         * @brief Construct an end iterator.
         */
        iterator(): _treap(NULL), _node(NULL) {}
        /**
         * @brief Construct an iterator from a node.
         * @param treap Owning container.
         * @param n Node to reference.
         */
        iterator(ImplicitTreap *treap, node *n): _treap(treap), _node(n) {}
        /**
         * @brief Copy-construct the iterator.
         * @param other Iterator to copy state from.
         */
        iterator(const iterator &other): _treap(other._treap), _node(other._node) {}
        /**
         * @brief Copy-assign from another iterator.
         * @param other Iterator to copy state from.
         * @return Reference to this iterator.
         */
        iterator &operator=(const iterator &other) {
            if (this != &other) {
                _treap = other._treap;
                _node = other._node;
            }
            return (*this);
        }
        /**
         * @brief Destroy the iterator.
         */
        ~iterator() {}

        /**
         * @brief Dereference to obtain a proxy referencing the node.
         * @return Proxy capable of reading or writing the value.
         */
        reference operator*() const {
            assert(_treap);
            assert(_node);
            return reference(_treap, _node);
        }
        /**
         * @brief Arrow operator exposing a pointer to the value.
         * @return Pointer to the node's value after lazy propagation.
         */
        pointer operator->() const {
            assert(_treap);
            assert(_node);
            _treap->materialize_node(_node);
            return (&_node->_value);
        }

        /**
         * @brief Advance to the next in-order node.
         * @return Reference to this iterator.
         */
        iterator &operator++() {
            assert(_treap);
            assert(_node);
            if (_node->_child[1]) {
                _node = _node->_child[1];
                while (_node->_child[0]) {
                    _treap->pushdown(_node);
                    _node = _node->_child[0];
                }
            } else {
                node* cur = _node;
                while (cur->_parent && cur == cur->_parent->_child[1]) {
                    cur = cur->_parent;
                }
                _node = cur->_parent;
            }
            return (*this);
        }
        /**
         * @brief Advance to the next node, returning prior state.
         * @return Iterator state before increment.
         */
        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return (tmp);
        }
        /**
         * @brief Move to the previous in-order node.
         * @return Reference to this iterator.
         */
        iterator &operator--() {
            assert(_treap);
            if (!_node) {
                _node = _treap->_root;
                if (!_node) {
                    return (*this);
                }
                while (_node->_child[1]) {
                    _treap->pushdown(_node);
                    _node = _node->_child[1];
                }
                return (*this);
            }
            if (_node->_child[0]) {
                _node = _node->_child[0];
                while (_node->_child[1]) {
                    _treap->pushdown(_node);
                    _node = _node->_child[1];
                }
            } else {
                node* cur = _node;
                while (cur->_parent && cur == cur->_parent->_child[0]) {
                    cur = cur->_parent;
                }
                _node = cur->_parent;
            }
            return (*this);
        }
        /**
         * @brief Move to the previous node, returning prior state.
         * @return Iterator state before decrement.
         */
        iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return (tmp);
        }
        /**
         * @brief Check iterator equality.
         * @param other Iterator to compare.
         * @return True if both reference the same node in the same container.
         */
        bool operator==(const iterator &other) const {
            return (_treap == other._treap && _node == other._node);
        }
        /**
         * @brief Check iterator inequality.
         * @param other Iterator to compare.
         * @return True if they differ in container or node.
         */
        bool operator!=(const iterator &other) const {
            return (!(*this == other));
        }

        /**
         * @brief Advance by a signed offset.
         * @param n Distance to move forward.
         * @return Reference to this iterator.
         */
        iterator &operator+=(difference_type n) {
            assert(_treap);
            difference_type size = static_cast<difference_type>(_treap->size());
            difference_type idx = static_cast<difference_type>(_treap->node_index(_node));
            difference_type target = idx + n;
            assert(target >= 0 && target <= size);
            if (target == size) {
                _node = NULL;
            } else {
                _node = _treap->find_node_by_index(static_cast<size_type>(target));
            }
            return (*this);
        }

        /**
         * @brief Move backward by a signed offset.
         * @param n Distance to move backward.
         * @return Reference to this iterator.
         */
        iterator &operator-=(difference_type n) {
            return (*this += -n);
        }

        /**
         * @brief Return an iterator advanced by a signed offset.
         * @param n Distance to move forward.
         * @return Iterator @p n steps ahead.
         */
        iterator operator+(difference_type n) const {
            iterator tmp(*this);
            tmp += n;
            return tmp;
        }

        /**
         * @brief Return an iterator moved backward by a signed offset.
         * @param n Distance to move backward.
         * @return Iterator @p n steps behind.
         */
        iterator operator-(difference_type n) const {
            iterator tmp(*this);
            tmp -= n;
            return tmp;
        }

        /**
         * @brief Compute distance between iterators.
         * @param other Iterator to subtract.
         * @return Signed number of steps between iterators.
         */
        difference_type operator-(const iterator &other) const {
            assert(_treap == other._treap);
            difference_type lhs = static_cast<difference_type>(_treap->node_index(_node));
            difference_type rhs = static_cast<difference_type>(_treap->node_index(other._node));
            return (lhs - rhs);
        }

        /**
         * @brief Compare iterator ordering.
         * @param other Iterator to compare.
         * @return True if this iterator precedes @p other.
         */
        bool operator<(const iterator &other) const {
            assert(_treap == other._treap);
            return (_treap->node_index(_node) < _treap->node_index(other._node));
        }

        /**
         * @brief Compare iterator ordering.
         * @param other Iterator to compare.
         * @return True if this iterator follows @p other.
         */
        bool operator>(const iterator &other) const {
            return (other < *this);
        }

        /**
         * @brief Compare iterator ordering for <=.
         * @param other Iterator to compare.
         * @return True if this iterator does not follow @p other.
         */
        bool operator<=(const iterator &other) const {
            return !(*this > other);
        }

        /**
         * @brief Compare iterator ordering for >=.
         * @param other Iterator to compare.
         * @return True if this iterator does not precede @p other.
         */
        bool operator>=(const iterator &other) const {
            return !(*this < other);
        }


        /**
         * @brief Expose the underlying node pointer for container internals.
         * @return Pointer to the node referenced by the iterator.
         */
        node *base_node() const {
            return _node;
        }


     private:
        ImplicitTreap *_treap;
        node *_node;
    };

    /**
     * @brief Obtain iterator to the first element.
     * @return Iterator addressing the smallest index.
     */
    iterator begin() {
        node *t = _root;
        if (!t) {
            return iterator(this, NULL);
        }
        while (t->_child[0]) {
            pushdown(t);
            t = t->_child[0];
        }
        return iterator(this, t);
    }

    /**
     * @brief Obtain iterator past the last element.
     * @return Iterator representing the end sentinel.
     */
    iterator end() {
        return iterator(this, NULL);
    }

    /**
     * @brief Obtain const iterator to the first element.
     * @return Const iterator addressing the smallest index.
     */
    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    /**
     * @brief Obtain const iterator past the last element.
     * @return Const iterator representing the end sentinel.
     */
    const_iterator end() const {
        return const_iterator(this, size());
    }

    /**
     * @brief Convenience alias for `begin()` in const context.
     * @return Const iterator to the first element.
     */
    const_iterator cbegin() const {
        return begin();
    }

    /**
     * @brief Convenience alias for `end()` in const context.
     * @return Const iterator past-the-end sentinel.
     */
    const_iterator cend() const {
        return end();
    }

private:
    /**
     * @brief Simple xorshift32 pseudo-random generator.
     */
    struct xorshift {
        unsigned int _state;
        /**
         * @brief Seed the generator.
         * @param _seed Initial state value.
         */
        xorshift(unsigned int _seed): _state(_seed) {}
        /**
         * @brief Produce the next random number.
         * @return 32-bit pseudo-random value.
         */
        unsigned int random() {
            _state ^= _state << 13;
            _state ^= _state >> 17;
            _state ^= _state << 5;
            return (_state);
        }
    };

    /**
     * @brief Treap node storing value and augmentation metadata.
     */
    struct node {
        value_type _value;
        value_type _acc;
        value_type _lazy;
        int _priority;
        size_type _cnt;
        bool _rev;
        node *_child[2];
        node *_parent;
        /**
         * @brief Construct a node initialized with a value and priority.
         * @param val Stored value.
         * @param priority Random priority used for heap ordering.
         */
        node(value_type val, int priority) {
            _value = val;
            _acc = operations::query_id();
            _lazy = operations::update_id();
            _priority = priority;
            _cnt = 1;
            _rev = false;
            _child[0] = NULL;
            _child[1] = NULL;
            _parent = NULL;
        }
    };

    typedef typename Allocator::template rebind<node>::other node_allocator_type;
    
    node *_root;
    node_allocator_type _node_alloc;
    xorshift _rnd;
    static const unsigned int seed = 1;

    /**
     * @brief Count nodes within a subtree.
     * @param t Root of the subtree.
     * @return Number of nodes contained in @p t.
     */
    size_type cnt(const node *t) const {
        if (!t) {
            return 0;
        }
        return (t->_cnt);
    }

    /**
     * @brief Retrieve aggregated sum for a subtree.
     * @param t Root of the subtree.
     * @return Aggregated value including pending lazy updates.
     */
    value_type acc(const node *t) const {
        if (!t) {
            return (operations::query_id());
        }
        return (t->_acc);
    }

    /**
     * @brief Recompute node metadata from its children.
     * @param t Node whose cached size and aggregate are updated.
     */
    void update(node *t) {
        if (!t) {
            return;
        }
        t->_cnt = 1 + cnt(t->_child[0]) + cnt(t->_child[1]);
        t->_acc = operations::query_op(operations::query_op(acc(t->_child[0]), t->_value), acc(t->_child[1]));
    }

    /**
     * @brief Convenience wrapper around update().
     * @param t Node whose metadata is refreshed.
     */
    void pushup(node *t) {
        if (!t) {
            return;
        }
        update(t);
    }

    /**
     * @brief Update a child's parent pointer.
     * @param child Node whose parent pointer is set.
     * @param parent Parent assigned to @p child.
     */
    void set_parent(node *child, node *parent) {
        if (child) {
            child->_parent = parent;
        }
    }

    /**
     * @brief Ensure both children reference their parent correctly.
     * @param t Node whose children will receive updated parent pointers.
     */
    void fix_children_parent(node *t) {
        if (!t) {
            return;
        }
        set_parent(t->_child[0], t);
        set_parent(t->_child[1], t);
    }

    /**
     * @brief Clear the parent pointer of a node.
     * @param t Node whose parent pointer becomes null.
     */
    void detach_parent(node *t) {
        if (t) {
            t->_parent = NULL;
        }
    }

    /**
     * @brief Propagate reverse and lazy flags to children while materializing @p t.
     * @param t Node whose deferred operations are pushed down.
     */
    void pushdown(node *t) {
        if (!t) {
            return;
        }
        fix_children_parent(t);
        if (t->_rev) {
            std::swap(t->_child[0], t->_child[1]);
            if (t->_child[0]) {
                t->_child[0]->_rev ^= 1;
            }
            if (t->_child[1]) {
                t->_child[1]->_rev ^= 1;
            }
            t->_rev = false;
        }
        if (t->_lazy != operations::update_id()) {
            if (t->_child[0]) {
                t->_child[0]->_lazy = operations::update_op(t->_child[0]->_lazy, t->_lazy);
                t->_child[0]->_acc = operations::apply(t->_child[0]->_acc, t->_lazy, static_cast<int>(cnt(t->_child[0])));
            }
            if (t->_child[1]) {
                t->_child[1]->_lazy = operations::update_op(t->_child[1]->_lazy, t->_lazy);
                t->_child[1]->_acc = operations::apply(t->_child[1]->_acc, t->_lazy, static_cast<int>(cnt(t->_child[1])));
            }
            t->_value = operations::apply(t->_value, t->_lazy, 1);
            t->_lazy = operations::update_id();
        }
    }

    /**
     * @brief Partition a treap around an index.
     * @param t Root of the treap to split.
     * @param key Number of nodes placed in the left partition.
     * @param l Resulting left treap.
     * @param r Resulting right treap.
     */
    void split(node *t, int key, node *&l, node *&r) {
        if (!t) {
            l = r = NULL;
        } else {
            pushdown(t);
            int cur_key = static_cast<int>(cnt(t->_child[0]));
            if (key > cur_key) {
                l = t;
                split(t->_child[1], key - cur_key - 1, l->_child[1], r);
                fix_children_parent(l);
                detach_parent(l);
                detach_parent(r);
                fix_children_parent(r);
            } else {
                r = t;
                split(t->_child[0], key, l, r->_child[0]);
                fix_children_parent(r);
                detach_parent(r);
                detach_parent(l);
                fix_children_parent(l);
            }
            pushup(t);
        }
    }

    /**
     * @brief Combine two treaps preserving in-order sequence.
     * @param t Destination root receiving the merged tree.
     * @param l Left treap whose keys precede @p r.
     * @param r Right treap whose keys follow @p l.
     */
    void merge(node *&t, node *l, node *r) {
        if (!l) {
            t = r;
            detach_parent(t);
            fix_children_parent(t);
            if (t) {
                pushdown(t);
                pushup(t);
            }
            return;
        }
        if (!r) {
            t = l;
            detach_parent(t);
            fix_children_parent(t);
            if (t) {
                pushdown(t);
                pushup(t);
            }
            return;
        }
        pushdown(l);
        pushdown(r);
        if (l->_priority > r->_priority) {
            t = l;
            merge(t->_child[1], l->_child[1], r);
        } else {
            t = r;
            merge(t->_child[0], l, r->_child[0]);
        }
        fix_children_parent(t);
        detach_parent(t);
        pushup(t);
    }

    /**
     * @brief Locate a mutable node by positional index.
     * @param pos Zero-based index to locate.
     * @return Pointer to the node or NULL if not found.
     */
    node* find_node_by_index(size_type pos) {
        node* t = _root;
        while (t) {
            pushdown(t);
            size_type left_cnt = cnt(t->_child[0]);
            if (pos == left_cnt) {
                return t;
            }
            if (pos < left_cnt) {
                t = t->_child[0];
            } else {
                pos = pos - left_cnt - 1;
                t = t->_child[1];
            }
        }
        return NULL;
    }

    /**
     * @brief Locate an immutable node by positional index.
     * @param pos Zero-based index to locate.
     * @return Pointer to the node or NULL if not found.
     */
    const node* find_node_by_index(size_type pos) const {
        return find_node_by_index_const(_root, pos, operations::update_id(), false);
    }

    /**
     * @brief Retrieve the value at an index accounting for lazy state.
     * @param t Current node being examined.
     * @param pos Relative index inside the subtree rooted at @p t.
     * @param pending_lazy Lazy increment accumulated from ancestors.
     * @param pending_rev Pending reversal flag inherited from ancestors.
     * @return Value materialized at the positional index.
     */
    value_type value_at_const(const node* t, size_type pos,
        value_type pending_lazy, bool pending_rev) const {
        assert(t);
        value_type next_lazy = operations::update_op(pending_lazy, t->_lazy);
        bool next_rev = pending_rev ^ t->_rev;
        const node* left = next_rev ? t->_child[1] : t->_child[0];
        size_type left_cnt = cnt(left);
        if (pos < left_cnt) {
            return value_at_const(left, pos, next_lazy, next_rev);
        }
        if (pos == left_cnt) {
            return operations::apply(t->_value, next_lazy, 1);
        }
        const node* right = next_rev ? t->_child[0] : t->_child[1];
        return value_at_const(right, pos - left_cnt - 1, next_lazy, next_rev);
    }

    /**
     * @brief Find a node by index while passing down pending flags.
     * @param t Current node in the traversal.
     * @param pos Relative index sought inside @p t.
     * @param pending_lazy Lazy increments inherited from ancestors.
     * @param pending_rev Pending reversal flag inherited from ancestors.
     * @return Pointer to the node at the requested index or NULL.
     */
    const node* find_node_by_index_const(const node* t, size_type pos,
        value_type pending_lazy, bool pending_rev) const {
        if (!t) {
            return NULL;
        }
        value_type next_lazy = operations::update_op(pending_lazy, t->_lazy);
        bool next_rev = pending_rev ^ t->_rev;
        const node* left = next_rev ? t->_child[1] : t->_child[0];
        size_type left_cnt = cnt(left);
        if (pos < left_cnt) {
            return find_node_by_index_const(left, pos, next_lazy, next_rev);
        }
        if (pos == left_cnt) {
            return t;
        }
        const node* right = next_rev ? t->_child[0] : t->_child[1];
        return find_node_by_index_const(right, pos - left_cnt - 1, next_lazy, next_rev);
    }

    /**
     * @brief Insert a node at a positional index within a subtree.
     * @param t Root pointer updated with the modified subtree.
     * @param key Target index where @p item is inserted.
     * @param item Node to insert; ownership transfers to the treap.
     */
    void insert(node *&t, int key, node *item) {
        node *left;
        node *right;
        split(t, key, left, right);
        merge(left, left, item);
        merge(t, left, right);
    }

    /**
     * @brief Remove the node at a positional index.
     * @param t Root pointer updated with the modified subtree.
     * @param key Positional index of the node to erase.
     */
    void erase(node *&t, int key) {
        node *left;
        node *right;
        node *mid;
        split(t, key, left, right);
        split(right, 1, mid, right);
        if (mid) {
            _node_alloc.destroy(mid);
            _node_alloc.deallocate(mid, 1);
        }
        merge(t, left, right);
    }

    /**
     * @brief Apply a lazy increment to a positional range within a subtree.
     * @param t Root pointer updated with the modified subtree.
     * @param l Inclusive range start relative to @p t.
     * @param r Exclusive range end relative to @p t.
     * @param val Lazy increment applied to the range.
     */
    void update(node *&t, size_type l, size_type r, value_type val) {
        if (l >= r) {
            return;
        }
        node *left;
        node *right;
        node *mid;
        split(t, l, left, right);
        split(right, r - l, mid, right);
        if (mid) {
            mid->_lazy = operations::update_op(mid->_lazy, val);
            mid->_acc = operations::apply(mid->_acc, val, static_cast<int>(cnt(mid)));
        }
        merge(right, mid, right);
        merge(t, left, right);
    }

    /**
     * @brief Compute the aggregated sum over [l, r) in a subtree.
     * @param t Root of the subtree being queried.
     * @param l Inclusive range start.
     * @param r Exclusive range end.
     * @return Aggregated range sum.
     */
    value_type query(node *t, size_type l, size_type r) {
        node *left;
        node *right;
        node *mid;
        split(t, l, left, right);
        split(right, r - l, mid, right);
        value_type res = operations::query_id();
        if (mid) {
            res = mid->_acc;
        }
        merge(right, mid, right);
        merge(t, left, right);
        return (res);
    }

    /**
     * @brief Calculate prefix sum up to a count within a subtree.
     * @param t Subtree root.
     * @param pos Number of elements from the left to include.
     * @return Aggregated sum of the first @p pos elements.
     */
    value_type prefix_sum(node *t, size_type pos) {
        if (!t || pos == 0) {
            return (operations::query_id());
        }
        pushdown(t);
        size_type left_cnt = cnt(t->_child[0]);
        if (pos <= left_cnt) {
            return prefix_sum(t->_child[0], pos);
        }
        value_type res = acc(t->_child[0]);
        if (pos == left_cnt + 1) {
            return operations::query_op(res, t->_value);
        }
        res = operations::query_op(res, t->_value);
        return operations::query_op(res, prefix_sum(t->_child[1], pos - left_cnt - 1));
    }


    void print_node(std::ostream &out, node *t, int depth) {
        if (!t) {
            out << "NULL";
            return;
        }
        pushdown(t);
        out << "(";
        print_node(out, t->_child[0], depth + 1);
        out << "(" << t->_value << "," << t->_priority <<  ")";
        print_node(out, t->_child[1], depth + 1);
        out << ")";
    }

    /**
     * @brief Locate the first index where a prefix sum crosses a threshold.
     * @param t Subtree root searched from the treap root context.
     * @param target Prefix sum threshold to reach.
     * @param strict True to search for > target, false for >= target.
     * @return Index of the crossing point or treap size if not found.
     */
    size_type prefix_bound(node *t, value_type target, bool strict) {
        size_type total = cnt(_root);
        size_type idx = 0;
        value_type sum = operations::query_id();
        node *cur = t;
        size_type result = total;
        // print_node(std::cout, _root, 0);
        // std::cout << std::endl;
        while (cur) {
            pushdown(cur);
            node *left = cur->_child[0];
            value_type left_sum = operations::query_op(sum, acc(left));
            // std::cout << "At idx " << idx << " with left count " << cnt(left)
            //     << " left_sum " << left_sum << " cur value " << cur->_value << std::endl;
            if ((!strict && left_sum >= target) || (strict && left_sum > target)) {
                // std::cout << "Going left" << std::endl;
                cur = left;
                continue;
            }
            idx += cnt(left);
            sum = left_sum;
            value_type with_current = operations::query_op(sum, cur->_value);
            // ++idx;
            if ((!strict && with_current >= target) || (strict && with_current > target)) {
                // std::cout << "Found at idx " << idx << std::endl;
                result = idx;
                break;
            }
            ++idx;
            sum = with_current;
            // std::cout << "Going right" << std::endl;
            cur = cur->_child[1];
        }
        return (result);
    }

    /**
     * @brief Compute the global index of a node.
     * @param n Node whose index is required.
     * @return Zero-based index or size() if @p n is NULL.
     */
    size_type node_index(node *n) {
        if (!n) {
            return cnt(_root);
        }
        pushdown_path(n);
        size_type idx = cnt(n->_child[0]);
        node *cur = n;
        while (cur->_parent) {
            node *parent = cur->_parent;
            pushdown(parent);
            if (cur == parent->_child[1]) {
                idx += cnt(parent->_child[0]) + 1;
            }
            cur = parent;
        }
        return idx;
    }

    /**
     * @brief Deep-copy a subtree, preserving augmentation state.
     * @param other Root of the subtree to clone.
     * @param parent Parent assigned to the newly created node.
     * @return Newly allocated subtree root.
     */
    node* clone_subtree(const node *other, node *parent) {
        if (!other) {
            return NULL;
        }
        node *new_node = _node_alloc.allocate(1);
        try {
            _node_alloc.construct(new_node, node(other->_value, other->_priority));
            new_node->_acc = other->_acc;
            new_node->_lazy = other->_lazy;
            new_node->_cnt = other->_cnt;
            new_node->_rev = other->_rev;
            new_node->_parent = parent;
            new_node->_child[0] = clone_subtree(other->_child[0], new_node);
            new_node->_child[1] = clone_subtree(other->_child[1], new_node);
        } catch (...) {
            if (new_node) {
                if (new_node->_child[0]) {
                    clear(new_node->_child[0]);
                }
                if (new_node->_child[1]) {
                    clear(new_node->_child[1]);
                }
                _node_alloc.destroy(new_node);
                _node_alloc.deallocate(new_node, 1);
            }
            throw;
        }
        return new_node;
    }

    /**
     * @brief Push lazy flags along the path from root to a node.
     * @param t Node whose ancestors are materialized.
     */
    void pushdown_path(node *t) {
        if (!t) {
            return;
        }
        node *buffer[64];
        size_type depth = 0;
        node *cur = t;
        while (cur && depth < sizeof(buffer) / sizeof(buffer[0])) {
            buffer[depth++] = cur;
            cur = cur->_parent;
        }
        if (!cur) {
            while (depth > 0) {
                pushdown(buffer[--depth]);
            }
            return;
        }
        std::vector<node *> path;
        path.reserve(depth + 16);
        for (size_type i = 0; i < depth; ++i) {
            path.push_back(buffer[i]);
        }
        while (cur) {
            path.push_back(cur);
            cur = cur->_parent;
        }
        for (typename std::vector<node *>::reverse_iterator it = path.rbegin();
            it != path.rend(); ++it) {
            pushdown(*it);
        }
    }

    /**
     * @brief Materialize a node by applying all pending flags from ancestors.
     * @param t Node to materialize.
     */
    void materialize_node(node *t) {
        if (!t) {
            return;
        }
        pushdown_path(t);
    }

    /**
     * @brief Retrieve a node's value after ensuring lazy propagation.
     * @param t Node whose value is requested.
     * @return Materialized value stored at @p t.
     */
    value_type node_value(node *t) {
        if (!t) {
            return (operations::query_id());
        }
        materialize_node(t);
        return (t->_value);
    }

    /**
     * @brief Recompute aggregates along the path to the root.
     * @param t Node from which to propagate updates upward.
     */
    void refresh_path(node *t) {
        node *cur = t;
        while (cur) {
            pushup(cur);
            cur = cur->_parent;
        }
    }

    /**
     * @brief Assign a new value to a node and refresh metadata.
     * @param t Node being updated.
     * @param val New value stored at the node.
     */
    void assign_node_value(node *t, value_type val) {
        if (!t) {
            return;
        }
        materialize_node(t);
        t->_value = val;
        pushup(t);
        refresh_path(t->_parent);
    }

    /**
     * @brief Count nodes with values less than a threshold.
     * @param t Subtree root examined as a binary search tree by value.
     * @param val Threshold value.
     * @return Number of nodes with value < @p val.
     */
    int lower_bound(node *t, value_type val) {
        if (!t) {
            return (0);
        }
        pushdown(t);
        if (t->_value >= val) {
            return (lower_bound(t->_child[0], val));
        }
        return (cnt(t->_child[0]) + 1 + lower_bound(t->_child[1], val));
    }

    /**
     * @brief Count nodes with values less than or equal to a threshold.
     * @param t Subtree root examined as a binary search tree by value.
     * @param val Threshold value.
     * @return Number of nodes with value <= @p val.
     */
    int upper_bound(node *t, value_type val) {
        if (!t) {
            return (0);
        }
        pushdown(t);
        if (t->_value > val) {
            return (upper_bound(t->_child[0], val));
        }
        return (cnt(t->_child[0]) + 1 + upper_bound(t->_child[1], val));
    }

    /**
     * @brief Apply a reversal flag to [l, r) inside a subtree.
     * @param t Root pointer updated with the modified subtree.
     * @param l Inclusive range start.
     * @param r Exclusive range end.
     */
    void reverse(node *t, size_type l, size_type r) {
        if (l >= r) {
            return;
        }
        node *left;
        node *right;
        node *mid;
        split(t, l, left, right);
        split(right, r - l, mid, right);
        mid->_rev ^= 1;
        merge(right, mid, right);
        merge(t, left, right);
    }

    /**
     * @brief Rotate [l, r) inside a subtree so index m becomes first.
     * @param t Root pointer updated with the modified subtree.
     * @param l Inclusive range start.
     * @param m Middle point that becomes the new front.
     * @param r Exclusive range end.
     */
    void rotate(node *t, size_type l, size_type m, size_type r) {
        if (l >= m || m >= r) {
            return;
        }
        reverse(t, l, r);
        reverse(t, l, l + r - m);
        reverse(t, l + r - m, r);
    }

    /**
     * @brief Recursively destroy all nodes in a subtree.
     * @param t Root of the subtree to delete.
     */
    void clear(node *t) {
        if (!t) {
            return;
        }
        clear(t->_child[0]);
        clear(t->_child[1]);
        _node_alloc.destroy(t);
        _node_alloc.deallocate(t, 1);
    }
};

/**
 * @brief Swap two implicit treaps using ADL.
 * @tparam T Value type stored in each treap.
 * @tparam Allocator Allocator type used by the treaps.
 * @param lhs First treap.
 * @param rhs Second treap.
 */
template<typename T, class Allocator>
inline void swap(ImplicitTreap<T, Allocator> &lhs, ImplicitTreap<T, Allocator> &rhs) {
    lhs.swap(rhs);
}

/**
 * @brief Symmetric addition operator for iterators.
 * @tparam T Value type stored in the treap.
 * @tparam Allocator Allocator type used by the treap.
 * @param n Signed distance added to the iterator.
 * @param it Iterator to offset.
 * @return Iterator advanced by @p n positions.
 */
template<typename T, class Allocator>
inline typename ImplicitTreap<T, Allocator>::iterator
operator+(typename ImplicitTreap<T, Allocator>::difference_type n,
        const typename ImplicitTreap<T, Allocator>::iterator &it) {
    typename ImplicitTreap<T, Allocator>::iterator tmp(it);
    tmp += n;
    return tmp;
}
