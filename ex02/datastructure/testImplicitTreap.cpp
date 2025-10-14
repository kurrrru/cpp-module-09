#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <numeric>
#include <vector>

#define private public
#define protected public
#include <ex02/datastructure/ImplicitTreap.hpp>
#undef private
#undef protected

namespace {

typedef ImplicitTreap<int>::size_type treap_size_type;

struct ValidationInfo {
    bool ok;
    treap_size_type count;
    int sum;
};

ValidationInfo validate_node(const ImplicitTreap<int>::node *node,
    const ImplicitTreap<int>::node *parent) {
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

void expect_sequence(ImplicitTreap<int> &treap,
    const std::vector<int> &expected) {
    materialize(treap);
    assert(treap.size() == expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i) {
        assert(treap[i] == expected[i]);
    }
    assert(validate_parent_links(treap));
    if (!expected.empty()) {
        int total = treap.query(0,
            static_cast<treap_size_type>(expected.size()));
        int expected_total = std::accumulate(expected.begin(),
            expected.end(), 0);
        assert(total == expected_total);
    }
}

void verify_all_ranges(ImplicitTreap<int> &treap,
    const std::vector<int> &mirror) {
    expect_sequence(treap, mirror);
    for (std::size_t l = 0; l < mirror.size(); ++l) {
        for (std::size_t r = l + 1; r <= mirror.size(); ++r) {
            int expected_sum = std::accumulate(mirror.begin() + l,
                mirror.begin() + r, 0);
            assert(treap.query(static_cast<treap_size_type>(l),
                static_cast<treap_size_type>(r)) == expected_sum);
        }
    }
}

void test_range_add_range_sum_behavior() {
    const std::size_t n = 16;
    ImplicitTreap<int> treap;
    std::vector<int> expected;
    expected.reserve(n);

    for (std::size_t i = 0; i < n; ++i) {
        int value = static_cast<int>(i);
        treap.insert(static_cast<treap_size_type>(i), value);
        expected.push_back(value);
    }
    expect_sequence(treap, expected);

    struct RangeUpdate {
        std::size_t l;
        std::size_t r;
        int delta;
    };

    const RangeUpdate updates[] = {
        {0u, n, 3},
        {5u, 12u, -2},
        {2u, 9u, 5},
        {4u, 7u, -7},
        {0u, 1u, 12}
    };
    const std::size_t update_count = sizeof(updates) / sizeof(updates[0]);

    for (std::size_t i = 0; i < update_count; ++i) {
        const RangeUpdate &u = updates[i];
        treap.update(static_cast<treap_size_type>(u.l),
            static_cast<treap_size_type>(u.r), u.delta);
        for (std::size_t j = u.l; j < u.r; ++j) {
            expected[j] += u.delta;
        }
        expect_sequence(treap, expected);
    }

    treap.update(static_cast<treap_size_type>(3), -4);
    expected[3] -= 4;
    expect_sequence(treap, expected);

    for (std::size_t l = 0; l < n; ++l) {
        for (std::size_t r = l + 1; r <= n; ++r) {
            int expected_sum = 0;
            for (std::size_t idx = l; idx < r; ++idx) {
                expected_sum += expected[idx];
            }
            assert(treap.query(static_cast<treap_size_type>(l),
                static_cast<treap_size_type>(r)) == expected_sum);
        }
    }
}

void benchmark_range_add_range_sum() {
    typedef ImplicitTreap<int>::size_type size_type;
    const size_type total_size = static_cast<size_type>(100000);
    const int operations = 20000;

    ImplicitTreap<int> treap;
    std::vector<int> mirror;
    mirror.reserve(total_size);

    std::clock_t build_start = std::clock();
    for (size_type i = 0; i < total_size; ++i) {
        int value = static_cast<int>(i % 97);
        treap.insert(i, value);
        mirror.push_back(value);
    }
    std::clock_t build_end = std::clock();

    std::srand(42);

    std::clock_t update_start = std::clock();
    for (int op = 0; op < operations; ++op) {
        size_type l = static_cast<size_type>(std::rand()
            % static_cast<int>(total_size));
        size_type max_len = total_size - l;
        if (max_len > static_cast<size_type>(64)) {
            max_len = static_cast<size_type>(64);
        }
        size_type length = static_cast<size_type>(1 + std::rand()
            % static_cast<int>(max_len));
        size_type r = l + length;
        int delta = (std::rand() % 11) - 5;
        treap.update(l, r, delta);
        for (size_type i = l; i < r; ++i) {
            mirror[i] += delta;
        }
    }
    std::clock_t update_end = std::clock();

    std::clock_t query_start = std::clock();
    long long checksum = 0;
    for (int op = 0; op < operations; ++op) {
        size_type l = static_cast<size_type>(std::rand()
            % static_cast<int>(total_size));
        size_type max_len = total_size - l;
        if (max_len > static_cast<size_type>(128)) {
            max_len = static_cast<size_type>(128);
        }
        size_type length = static_cast<size_type>(1 + std::rand()
            % static_cast<int>(max_len));
        size_type r = l + length;
        int treap_sum = treap.query(l, r);
        int expected_sum = 0;
        for (size_type i = l; i < r; ++i) {
            expected_sum += mirror[i];
        }
        assert(treap_sum == expected_sum);
        checksum += treap_sum;
    }
    std::clock_t query_end = std::clock();

    double build_ms = static_cast<double>(build_end - build_start)
        * 1000.0 / CLOCKS_PER_SEC;
    double update_ms = static_cast<double>(update_end - update_start)
        * 1000.0 / CLOCKS_PER_SEC;
    double query_ms = static_cast<double>(query_end - query_start)
        * 1000.0 / CLOCKS_PER_SEC;

    std::cout << "Benchmark Range Add/Range Sum (N=" << total_size
              << ", ops=" << operations << ")\n";
    std::cout << " build:  " << build_ms << " ms\n";
    std::cout << " update: " << update_ms << " ms\n";
    std::cout << " query:  " << query_ms << " ms\n";
    std::cout << " checksum: " << checksum << '\n';
}

void test_mixed_operations_range_sum() {
    ImplicitTreap<int> treap;
    std::vector<int> mirror;

    for (int i = 0; i < 12; ++i) {
        int value = i * 3 - 5;
        treap.insert(static_cast<treap_size_type>(i), value);
        mirror.push_back(value);
    }

    verify_all_ranges(treap, mirror);

    treap[3] = 100;
    mirror[3] = 100;
    verify_all_ranges(treap, mirror);

    treap.update(2, 8, -5);
    for (std::size_t i = 2; i < 8; ++i) {
        mirror[i] -= 5;
    }
    verify_all_ranges(treap, mirror);

    treap.update(static_cast<treap_size_type>(mirror.size() - 1), 7);
    mirror.back() += 7;
    verify_all_ranges(treap, mirror);

    treap.reverse(1, static_cast<treap_size_type>(mirror.size()));
    std::reverse(mirror.begin() + 1, mirror.end());
    verify_all_ranges(treap, mirror);

    treap.rotate(0, 4, static_cast<treap_size_type>(mirror.size()));
    std::rotate(mirror.begin(), mirror.begin() + 4, mirror.end());
    verify_all_ranges(treap, mirror);

    treap.erase(5);
    mirror.erase(mirror.begin() + 5);
    verify_all_ranges(treap, mirror);

    treap.insert(2, -11);
    mirror.insert(mirror.begin() + 2, -11);
    verify_all_ranges(treap, mirror);

    treap[2] = treap[2] + 4;
    mirror[2] += 4;
    verify_all_ranges(treap, mirror);

    if (mirror.size() >= 6) {
        treap.update(1, 6, 3);
        for (std::size_t i = 1; i < 6; ++i) {
            mirror[i] += 3;
        }
        verify_all_ranges(treap, mirror);
    }

    treap.update(static_cast<treap_size_type>(mirror.size() - 1), -8);
    mirror.back() -= 8;
    verify_all_ranges(treap, mirror);

    if (mirror.size() >= 3) {
        treap.reverse(0, 3);
        std::reverse(mirror.begin(), mirror.begin() + 3);
        verify_all_ranges(treap, mirror);
    }

    if (mirror.size() > 3) {
        treap_size_type block_end = static_cast<treap_size_type>(
            std::min<std::size_t>(mirror.size(), 7));
        if (block_end > 3) {
            treap.rotate(1, 3, block_end);
            std::rotate(mirror.begin() + 1, mirror.begin() + 3,
                mirror.begin() + block_end);
            verify_all_ranges(treap, mirror);
        }
    }

    if (mirror.size() > 2) {
        treap_size_type erase_pos = static_cast<treap_size_type>(
            mirror.size() - 2);
        treap.erase(erase_pos);
        mirror.erase(mirror.end() - 2);
        verify_all_ranges(treap, mirror);
    }

    treap.insert(static_cast<treap_size_type>(mirror.size()), 42);
    mirror.push_back(42);
    verify_all_ranges(treap, mirror);

    treap.update(0, static_cast<treap_size_type>(mirror.size()), 1);
    for (std::size_t i = 0; i < mirror.size(); ++i) {
        mirror[i] += 1;
    }
    verify_all_ranges(treap, mirror);
}

}  // namespace

int main() {
    ImplicitTreap<int> treap;
    std::vector<int> expected;
    const int initial_raw[] = {5, 3, 8, 1, 4, 7, 9, 2, 6};
    const std::size_t initial_count = sizeof(initial_raw)
        / sizeof(initial_raw[0]);

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
    int expected_sum = std::accumulate(expected.begin() + ql,
        expected.begin() + qr, 0);
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
    const ImplicitTreap<int>::size_type rev_r
        = static_cast<ImplicitTreap<int>::size_type>(expected.size() - 1);
    treap.reverse(rev_l, rev_r);
    std::reverse(expected.begin() + rev_l, expected.begin() + rev_r);
    expect_sequence(treap, expected);

    treap.rotate(0, 3,
        static_cast<ImplicitTreap<int>::size_type>(expected.size()));
    std::rotate(expected.begin(), expected.begin() + 3, expected.end());
    expect_sequence(treap, expected);

    treap.clear();
    expected.clear();
    assert(treap.size() == 0);
    assert(validate_parent_links(treap));

    treap.insert(0, 42);
    expected.push_back(42);
    expect_sequence(treap, expected);

    ImplicitTreap<int> treap2(5, 7);
    std::vector<int> expected2(5, 7);
    treap2[2] = 10;
    expected2[2] = 10;
    expect_sequence(treap2, expected2);
    assert(treap2.query(0, 5) == 7 * 4 + 10);
    treap2[0] = 3;
    expected2[0] = 3;
    treap2.update(1, 4, 1);
    for (std::size_t i = 1; i < 4; ++i) {
        expected2[i] += 1;
    }
    expect_sequence(treap2, expected2);
    assert(treap2.query(0, 5) == 3 + 8 + 11 + 8 + 7);

    treap2.insert(3, 20);
    expected2.insert(expected2.begin() + 3, 20);
    treap2.reverse(1, 4);
    std::reverse(expected2.begin() + 1, expected2.begin() + 4);
    expect_sequence(treap2, expected2);

    ImplicitTreap<int> treap3(treap2);
    expect_sequence(treap3, expected2);

    test_range_add_range_sum_behavior();
    test_mixed_operations_range_sum();
    benchmark_range_add_range_sum();

    const int vec_raw[] = {0, 1, 1, 0};
    std::vector<int> vec(vec_raw, vec_raw + sizeof(vec_raw)
        / sizeof(vec_raw[0]));
    ImplicitTreap<int> treap4(vec);
    assert(treap4.query_lower_bound(0, treap4.size(), 0) == 0);
    assert(treap4.query_lower_bound(0, treap4.size(), 1) == 1);
    assert(treap4.query_lower_bound(0, treap4.size(), 2) == 2);
    assert(treap4.query_lower_bound(0, treap4.size(), 3) == 4);
    assert(treap4.query_upper_bound(0, treap4.size(), 0) == 1);
    assert(treap4.query_upper_bound(0, treap4.size(), 1) == 2);
    assert(treap4.query_upper_bound(0, treap4.size(), 2) == 4);
    assert(treap4.query_upper_bound(0, treap4.size(), 3) == 4);

    const int vec2_raw[] = {1, 1, 2, 4, 4, 4, 5, 6, 8, 9};
    std::vector<int> vec2(vec2_raw, vec2_raw + sizeof(vec2_raw)
        / sizeof(vec2_raw[0]));
    ImplicitTreap<int> treap5(vec2);
    assert(treap5.lower_bound(0, treap5.size(), 0) == 0);
    assert(treap5.lower_bound(0, treap5.size(), 1) == 0);
    assert(treap5.lower_bound(0, treap5.size(), 2) == 2);
    assert(treap5.lower_bound(0, treap5.size(), 3) == 3);
    assert(treap5.lower_bound(0, treap5.size(), 4) == 3);
    assert(treap5.lower_bound(0, treap5.size(), 5) == 6);
    assert(treap5.lower_bound(0, treap5.size(), 6) == 7);
    assert(treap5.lower_bound(0, treap5.size(), 7) == 8);
    assert(treap5.lower_bound(0, treap5.size(), 8) == 8);
    assert(treap5.lower_bound(0, treap5.size(), 9) == 9);
    assert(treap5.lower_bound(0, treap5.size(), 10) == 10);
    assert(treap5.upper_bound(0, treap5.size(), 0) == 0);
    assert(treap5.upper_bound(0, treap5.size(), 1) == 2);
    assert(treap5.upper_bound(0, treap5.size(), 2) == 3);
    assert(treap5.upper_bound(0, treap5.size(), 3) == 3);
    assert(treap5.upper_bound(0, treap5.size(), 4) == 6);
    assert(treap5.upper_bound(0, treap5.size(), 5) == 7);
    assert(treap5.upper_bound(0, treap5.size(), 6) == 8);
    assert(treap5.upper_bound(0, treap5.size(), 7) == 8);
    assert(treap5.upper_bound(0, treap5.size(), 8) == 9);
    assert(treap5.upper_bound(0, treap5.size(), 9) == 10);
    assert(treap5.upper_bound(0, treap5.size(), 10) == 10);

    for (ImplicitTreap<int>::iterator it = treap5.begin();
        it != treap5.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
    for (ImplicitTreap<int>::const_iterator it = treap5.begin();
        it != treap5.end(); ++it) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;

    // reverse iteration
    for (ImplicitTreap<int>::iterator it = treap5.end();
        it != treap5.begin();) {
        --it;
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
    for (ImplicitTreap<int>::const_iterator it = treap5.end();
        it != treap5.begin();) {
        --it;
        std::cout << *it << ' ';
    }
    std::cout << std::endl;

    return 0;
}
