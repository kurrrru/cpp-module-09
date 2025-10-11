// - operator[]
// - iterator
// - i番目のbiggerを取得 // RSQ? accの上で二分探索

#pragma once
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
 public:
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
    ImplicitTreap(ImplicitTreap &other);
    ImplicitTreap &operator=(ImplicitTreap &other);
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
        assert(size >= 0);
        for (size_type i = 0; i < size; i++) {
            insert(i, val);
        }
    }

    value_type query(size_type l, size_type r) {
        size_type current_size = size();
        assert(l <= r && r <= current_size);
        return (query(_root, l, r));
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

    const_reference operator[](size_type pos) const {
        size_type current_size = size();
        assert(pos < current_size);
        const node* target_node = find_node_by_index(pos);
        return target_node->_value;
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

    // int lower_bound(size_type l, size_type r, value_type val) {
    //     return (lower_bound(_root, val));
    // }

    // int upper_bound(size_type l, size_type r, value_type val) {
    //     return (upper_bound(_root, val));
    // }

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

    class iterator: public std::iterator<std::random_access_iterator_tag, value_type> {
     public:
        iterator(): _treap(NULL), _node(NULL) {}
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
            assert(_node);
            return (_node->_value);
        }
        pointer operator->() const {
            assert(_node);
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
    };

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
        mutable value_type _value;
        mutable value_type _acc;
        mutable value_type _lazy;
        int _priority;
        size_type _cnt;
        mutable bool _rev;
        mutable node *_child[2];
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

    void set_parent(node *child, node *parent) const {
        if (child) {
            child->_parent = parent;
        }
    }

    void fix_children_parent(node *t) const {
        if (!t) {
            return;
        }
        set_parent(t->_child[0], t);
        set_parent(t->_child[1], t);
    }

    void detach_parent(node *t) const {
        if (t) {
            t->_parent = NULL;
        }
    }

    void pushdown(node *t) const {
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
                pushup(t);
            }
            return;
        }
        if (!r) {
            t = l;
            detach_parent(t);
            fix_children_parent(t);
            if (t) {
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

    const node* find_node_by_index(size_type pos) const {
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

    // int lower_bound(node *t, value_type val) {
    //     if (!t) {
    //         return (0);
    //     }
    //     pushdown(t);
    //     if (t->_value >= val) {
    //         return (lower_bound(t->_child[0], val));
    //     }
    //     return (cnt(t->_child[0]) + 1 + lower_bound(t->_child[1], val));
    // }

    // int upper_bound(node *t, value_type val) {
    //     if (!t) {
    //         return (0);
    //     }
    //     pushdown(t);
    //     if (t->_value > val) {
    //         return (upper_bound(t->_child[0], val));
    //     }
    //     return (cnt(t->_child[0]) + 1 + upper_bound(t->_child[1], val));
    // }

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
