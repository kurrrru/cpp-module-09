#define private public
#define protected public
#include "ImplicitTreap.hpp"
#undef private
#undef protected

#include <algorithm>
#include <cassert>
#include <numeric>
#include <vector>

namespace {

typedef ImplicitTreap<int>::size_type treap_size_type;

struct ValidationInfo {
    bool ok;
    treap_size_type count;
    int sum;
};

ValidationInfo validate_node(const ImplicitTreap<int>::node *node, const ImplicitTreap<int>::node *parent) {
    if (!node) {
        ValidationInfo info;
        info.ok = true;
        info.count = 0;
        info.sum = 0;
        return info;
    }
    bool ok = (node->_parent == parent);
    ValidationInfo left = validate_node(node->_child[0], node);
    ValidationInfo right = validate_node(node->_child[1], node);
    treap_size_type expected_cnt = left.count + right.count + 1;
    int expected_sum = left.sum + node->_value + right.sum;
    ok = ok && left.ok && right.ok;
    ok = ok && (node->_cnt == expected_cnt);
    ok = ok && (node->_acc == expected_sum);
    ValidationInfo result;
    result.ok = ok;
    result.count = expected_cnt;
    result.sum = expected_sum;
    return result;
}

bool validate_parent_links(const ImplicitTreap<int> &treap) {
    ValidationInfo info = validate_node(treap._root, NULL);
    return info.ok && info.count == treap.size();
}

void materialize(ImplicitTreap<int> &treap) {
    for (ImplicitTreap<int>::size_type i = 0; i < treap.size(); ++i) {
        (void)treap[i];
    }
}

void expect_sequence(ImplicitTreap<int> &treap, const std::vector<int> &expected) {
    materialize(treap);
    assert(treap.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        assert(treap[i] == expected[i]);
    }
    assert(validate_parent_links(treap));
    if (!expected.empty()) {
        int total = treap.query(0, static_cast<treap_size_type>(expected.size()));
        int expected_total = std::accumulate(expected.begin(), expected.end(), 0);
        assert(total == expected_total);
    }
}

}  // namespace

int main() {
    ImplicitTreap<int> treap;
    std::vector<int> expected;
    const int initial_raw[] = {5, 3, 8, 1, 4, 7, 9, 2, 6};
    const std::size_t initial_count = sizeof(initial_raw) / sizeof(initial_raw[0]);

    for (std::size_t i = 0; i < initial_count; ++i) {
        treap.insert(i, initial_raw[i]);
        expected.push_back(initial_raw[i]);
    }
    expect_sequence(treap, expected);

    treap.insert(3, 100);
    expected.insert(expected.begin() + 3, 100);
    expect_sequence(treap, expected);

    treap.erase(5);
    expected.erase(expected.begin() + 5);
    expect_sequence(treap, expected);

    const ImplicitTreap<int>::size_type ql = 2;
    const ImplicitTreap<int>::size_type qr = 6;
    int expected_sum = std::accumulate(expected.begin() + ql, expected.begin() + qr, 0);
    int treap_sum = treap.query(ql, qr);
    assert(treap_sum == expected_sum);

    treap.update(1, 5, 4);
    for (std::size_t i = 1; i < 5; ++i) {
        expected[i] += 4;
    }
    expect_sequence(treap, expected);

    treap.update(0, -2);
    expected[0] -= 2;
    expect_sequence(treap, expected);

    const ImplicitTreap<int>::size_type rev_l = 1;
    const ImplicitTreap<int>::size_type rev_r = static_cast<ImplicitTreap<int>::size_type>(expected.size() - 1);
    treap.reverse(rev_l, rev_r);
    std::reverse(expected.begin() + rev_l, expected.begin() + rev_r);
    expect_sequence(treap, expected);

    treap.rotate(0, 3, static_cast<ImplicitTreap<int>::size_type>(expected.size()));
    std::rotate(expected.begin(), expected.begin() + 3, expected.end());
    expect_sequence(treap, expected);

    treap.clear();
    expected.clear();
    assert(treap.size() == 0);
    assert(validate_parent_links(treap));

    treap.insert(0, 42);
    expected.push_back(42);
    expect_sequence(treap, expected);

    return 0;
}