// - operator[]
// - iterator
// - i番目のbiggerを取得 // RSQ? accの上で二分探索

#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <memory>
#include <vector>

struct monoid_range_add_range_sum {
    static int query_op(int a, int b) {
        return (a + b);
    }
    static int query_id() {
        return (0);
    }
    static int update_op(int a, int b) {
        return (a + b);
    }
    static int update_id() {
        return (0);
    }
    static int apply(int a, int b, int len) {
        return (b == update_id() ? a : a + b * len);
    }
};

template<typename T, class Allocator = std::allocator<T> >
class ImplicitTreap {
 private:
    struct node;
 public:
    class value_proxy;
    typedef T                                        value_type;
    typedef Allocator                                allocator_type;
    typedef typename Allocator::reference            reference;
    typedef typename Allocator::const_reference      const_reference;
    typedef typename Allocator::pointer              pointer;
    typedef typename Allocator::const_pointer        const_pointer;
    typedef std::size_t                              size_type;
    typedef std::ptrdiff_t                           difference_type;

    typedef monoid_range_add_range_sum               operations;

    ImplicitTreap(const Allocator &alloc = Allocator()) 
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
    }
    ~ImplicitTreap() {
        clear();
    }

    ImplicitTreap(std::vector<value_type> &vec, const Allocator &alloc = Allocator())
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
        for (size_type i = 0; i < vec.size(); i++) {
            insert(i, vec[i]);
        }
    }
    ImplicitTreap(size_type size, value_type val, const Allocator &alloc = Allocator())
        : _root(NULL), _node_alloc(alloc), _rnd(seed) {
        for (size_type i = 0; i < size; i++) {
            insert(i, val);
        }
    }

    ImplicitTreap(const ImplicitTreap &other)
        : _root(NULL), _node_alloc(other._node_alloc), _rnd(other._rnd) {
        _root = clone_subtree(other._root, NULL);
    }

    ImplicitTreap &operator=(const ImplicitTreap &other) {
        if (this != &other) {
            ImplicitTreap tmp(other);
            swap(tmp);
        }
        return (*this);
    }

    void swap(ImplicitTreap &other) {
        std::swap(_root, other._root);
        std::swap(_node_alloc, other._node_alloc);
        std::swap(_rnd, other._rnd);
    }

    friend void swap(ImplicitTreap &lhs, ImplicitTreap &rhs) {
        lhs.swap(rhs);
    }

    value_type query(size_type l, size_type r) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        return (query(_root, l, r));
    }

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

    void update(size_type pos, value_type val) {
        size_type current_size = size();
        assert(pos < current_size);
        update(_root, pos, pos + 1, val);
    }

    void update(size_type l, size_type r, value_type val) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        update(_root, l, r, val);
    }

    value_proxy operator[](size_type pos) {
        size_type current_size = size();
        assert(pos < current_size);
        node* target_node = find_node_by_index(pos);
        assert(target_node);
        return value_proxy(this, target_node);
    }

    value_type operator[](size_type pos) const {
        size_type current_size = size();
        assert(pos < current_size);
        return value_at_const(_root, pos, operations::update_id(), false);
    }

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

    void erase(size_type pos) {
        size_type current_size = size();
        assert(pos < current_size);
        erase(_root, pos);
    }

    size_type size() const {
        if (!_root) {
            return 0;
        }
        return cnt(_root);
    }

    int lower_bound(size_type l, size_type r, value_type val) {
        return (lower_bound(_root, val));
    }

    int upper_bound(size_type l, size_type r, value_type val) {
        return (upper_bound(_root, val));
    }

    void reverse(size_type l, size_type r) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        reverse(_root, l, r);
    }

    void rotate(size_type l, size_type m, size_type r) {
        size_type current_size = size();
        assert(l <= m && m <= r && r <= current_size);
        rotate(_root, l, m, r);
    }

    void clear() {
        clear(_root);
        _root = NULL;
    }

    class const_iterator: public std::iterator<std::bidirectional_iterator_tag, value_type> {
     public:
        const_iterator(): _treap(NULL), _index(0), _cache() {}
        const_iterator(const ImplicitTreap *treap, size_type index)
            : _treap(treap), _index(index), _cache() {}
        const_iterator(const const_iterator &other)
            : _treap(other._treap), _index(other._index), _cache(other._cache) {}
        const_iterator &operator=(const const_iterator &other) {
            if (this != &other) {
                _treap = other._treap;
                _index = other._index;
                _cache = other._cache;
            }
            return (*this);
        }
        ~const_iterator() {}

        value_type operator*() const {
            assert(_treap);
            assert(_index < _treap->size());
            _cache = (*_treap)[_index];
            return _cache;
        }
        const value_type *operator->() const {
            assert(_treap);
            assert(_index < _treap->size());
            _cache = (*_treap)[_index];
            return &_cache;
        }

        const_iterator &operator++() {
            assert(_treap);
            assert(_index < _treap->size());
            ++_index;
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return (tmp);
        }
        const_iterator &operator--() {
            assert(_treap);
            assert(_index > 0);
            --_index;
            return (*this);
        }
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return (tmp);
        }
        bool operator==(const const_iterator &other) const {
            return (_treap == other._treap && _index == other._index);
        }
        bool operator!=(const const_iterator &other) const {
            return (!(*this == other));
        }

        const_iterator &operator+=(difference_type n) {
            assert(_treap);
            difference_type target = static_cast<difference_type>(_index) + n;
            assert(target >= 0 && target <= static_cast<difference_type>(_treap->size()));
            _index = static_cast<size_type>(target);
            return (*this);
        }

        const_iterator &operator-=(difference_type n) {
            return (*this += -n);
        }

        const_iterator operator+(difference_type n) const {
            const_iterator tmp(*this);
            tmp += n;
            return tmp;
        }

        const_iterator operator-(difference_type n) const {
            const_iterator tmp(*this);
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const const_iterator &other) const {
            assert(_treap == other._treap);
            return static_cast<difference_type>(_index) - static_cast<difference_type>(other._index);
        }

        bool operator<(const const_iterator &other) const {
            assert(_treap == other._treap);
            return _index < other._index;
        }

        bool operator>(const const_iterator &other) const {
            return (other < *this);
        }

        bool operator<=(const const_iterator &other) const {
            return !(*this > other);
        }

        bool operator>=(const const_iterator &other) const {
            return !(*this < other);
        }

     private:
        const ImplicitTreap *_treap;
        size_type _index;
        mutable value_type _cache;
    };

    class value_proxy {
     public:
        value_proxy(ImplicitTreap *treap, node *n)
            : _treap(treap), _node(n) {}
        value_proxy(const value_proxy &other)
            : _treap(other._treap), _node(other._node) {}
        value_proxy &operator=(const value_type &val) {
            assert(_treap);
            assert(_node);
            _treap->assign_node_value(_node, val);
            return (*this);
        }
        value_proxy &operator=(const value_proxy &other) {
            return (*this = static_cast<value_type>(other));
        }
        operator value_type() const {
            assert(_treap);
            assert(_node);
            return (_treap->node_value(_node));
        }
        value_type *operator->() {
            assert(_treap);
            assert(_node);
            _treap->materialize_node(_node);
            return (&_node->_value);
        }
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

    class iterator {
     public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename ImplicitTreap::value_type value_type;
        typedef typename ImplicitTreap::difference_type difference_type;
        typedef value_proxy reference;
        typedef value_type* pointer;

        iterator(): _treap(NULL), _node(NULL) {}
        iterator(ImplicitTreap *treap, node *n): _treap(treap), _node(n) {}
        iterator(const iterator &other): _treap(other._treap), _node(other._node) {}
        iterator &operator=(const iterator &other) {
            if (this != &other) {
                _treap = other._treap;
                _node = other._node;
            }
            return (*this);
        }
        ~iterator() {}

        reference operator*() const {
            assert(_treap);
            assert(_node);
            return reference(_treap, _node);
        }
        pointer operator->() const {
            assert(_treap);
            assert(_node);
            _treap->materialize_node(_node);
            return (&_node->_value);
        }

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
        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return (tmp);
        }
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
        iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return (tmp);
        }
        bool operator==(const iterator &other) const {
            return (_treap == other._treap && _node == other._node);
        }
        bool operator!=(const iterator &other) const {
            return (!(*this == other));
        }

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

        iterator &operator-=(difference_type n) {
            return (*this += -n);
        }

        iterator operator+(difference_type n) const {
            iterator tmp(*this);
            tmp += n;
            return tmp;
        }

        iterator operator-(difference_type n) const {
            iterator tmp(*this);
            tmp -= n;
            return tmp;
        }

        difference_type operator-(const iterator &other) const {
            assert(_treap == other._treap);
            difference_type lhs = static_cast<difference_type>(_treap->node_index(_node));
            difference_type rhs = static_cast<difference_type>(_treap->node_index(other._node));
            return (lhs - rhs);
        }

        bool operator<(const iterator &other) const {
            assert(_treap == other._treap);
            return (_treap->node_index(_node) < _treap->node_index(other._node));
        }

        bool operator>(const iterator &other) const {
            return (other < *this);
        }

        bool operator<=(const iterator &other) const {
            return !(*this > other);
        }

        bool operator>=(const iterator &other) const {
            return !(*this < other);
        }

        friend iterator operator+(difference_type n, const iterator &it) {
            iterator tmp(it);
            tmp += n;
            return tmp;
        }

     private:
        ImplicitTreap *_treap;
        node *_node;
    };

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

    iterator end() {
        return iterator(this, NULL);
    }

    const_iterator begin() const {
        return const_iterator(this, 0);
    }

    const_iterator end() const {
        return const_iterator(this, size());
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

private:
    struct xorshift {
        unsigned int _state;
        xorshift(unsigned int _seed): _state(_seed) {}
        unsigned int random() {
            _state ^= _state << 13;
            _state ^= _state >> 17;
            _state ^= _state << 5;
            return (_state);
        }
    };

    struct node {
        value_type _value;
        value_type _acc;
        value_type _lazy;
        int _priority;
        size_type _cnt;
        bool _rev;
        node *_child[2];
        node *_parent;
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
    
    node *_root = NULL;
    node_allocator_type _node_alloc;
    xorshift _rnd;
    static const unsigned int seed = 1;

    size_type cnt(const node *t) const {
        if (!t) {
            return 0;
        }
        return (t->_cnt);
    }

    value_type acc(const node *t) const {
        if (!t) {
            return (operations::query_id());
        }
        return (t->_acc);
    }

    void update(node *t) {
        if (!t) {
            return;
        }
        t->_cnt = 1 + cnt(t->_child[0]) + cnt(t->_child[1]);
        t->_acc = operations::query_op(operations::query_op(acc(t->_child[0]), t->_value), acc(t->_child[1]));
    }

    void pushup(node *t) {
        if (!t) {
            return;
        }
        update(t);
    }

    void set_parent(node *child, node *parent) {
        if (child) {
            child->_parent = parent;
        }
    }

    void fix_children_parent(node *t) {
        if (!t) {
            return;
        }
        set_parent(t->_child[0], t);
        set_parent(t->_child[1], t);
    }

    void detach_parent(node *t) {
        if (t) {
            t->_parent = NULL;
        }
    }

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
            fix_children_parent(t);
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

    const node* find_node_by_index(size_type pos) const {
        return find_node_by_index_const(_root, pos, operations::update_id(), false);
    }

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

    void insert(node *&t, int key, node *item) {
        node *left;
        node *right;
        split(t, key, left, right);
        merge(left, left, item);
        merge(t, left, right);
    }

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

    size_type prefix_bound(node *t, value_type target, bool strict) {
        size_type total = cnt(_root);
        size_type idx = 0;
        value_type sum = operations::query_id();
        node *cur = t;
        size_type result = total;
        while (cur) {
            pushdown(cur);
            node *left = cur->_child[0];
            value_type left_sum = operations::query_op(sum, acc(left));
            if ((!strict && left_sum >= target) || (strict && left_sum > target)) {
                cur = left;
                continue;
            }
            idx += cnt(left);
            sum = left_sum;
            value_type with_current = operations::query_op(sum, cur->_value);
            ++idx;
            if ((!strict && with_current >= target) || (strict && with_current > target)) {
                result = idx;
                break;
            }
            sum = with_current;
            cur = cur->_child[1];
        }
        return (result);
    }

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

    void pushdown_path(node *t) {
        if (!t) {
            return;
        }
        std::vector<node *> path;
        node *cur = t;
        while (cur) {
            path.push_back(cur);
            cur = cur->_parent;
        }
        for (typename std::vector<node *>::reverse_iterator it = path.rbegin();
            it != path.rend(); ++it) {
            pushdown(*it);
        }
    }

    void materialize_node(node *t) {
        if (!t) {
            return;
        }
        pushdown_path(t);
    }

    value_type node_value(node *t) {
        if (!t) {
            return (operations::query_id());
        }
        materialize_node(t);
        return (t->_value);
    }

    void refresh_path(node *t) {
        node *cur = t;
        while (cur) {
            pushup(cur);
            cur = cur->_parent;
        }
    }

    void assign_node_value(node *t, value_type val) {
        if (!t) {
            return;
        }
        materialize_node(t);
        t->_value = val;
        pushup(t);
        refresh_path(t->_parent);
    }

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

    void rotate(node *t, size_type l, size_type m, size_type r) {
        if (l >= m || m >= r) {
            return;
        }
        reverse(t, l, r);
        reverse(t, l, l + r - m);
        reverse(t, l + r - m, r);
    }

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
