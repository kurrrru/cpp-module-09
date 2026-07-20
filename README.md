[Japanese version](README.ja.md)
# cpp-module-09

A C++ repository implementing Bitcoin price conversion via multi-calendar-system date conversion, a Reverse Polish Notation (RPN) calculator based on arbitrary-precision rational numbers, and integer sorting via the Ford-Johnson algorithm with a multi-algorithm comparison benchmark (based on the 42 Cursus curriculum).

## Layout

```
ex00/  Bitcoin Exchange — Bitcoin price conversion via multi-calendar-system date conversion
ex01/  RPN               — Reverse Polish Notation arithmetic with arbitrary-precision rational numbers
ex02/  PmergeMe           — Ford-Johnson (merge-insertion) sort with a multi-algorithm comparison benchmark
toolbox/ Shared infrastructure used across all three exercises (logger, string, math utilities)
```

### ex00: Bitcoin Price Conversion via Multi-Calendar-System Date Conversion (BitcoinExchange)

Date handling is built on top of an abstract interface, `ICalendarSystem` (`ex00/calendar_system/ICalendarSystem.hpp`), which converts dates to and from a serial value (`to_serial_date` / `from_serial_date`). The following calendar systems are implemented against this interface:

- Gregorian calendar (`GregorianCalendar`)
- Julian calendar (`JulianCalendar`)
- French Republican calendar (`FrenchRepublicanCalendar`)
- Ethiopian calendar (`EthiopianCalendar`)
- Non-proleptic Gregorian calendar (`NonProlepticGregorianCalendar`)

The program itself uses the Gregorian calendar.

### ex01: Reverse Polish Notation Arithmetic with Arbitrary-Precision Rational Numbers (RPN)

Numeric operations use `BigInt` (an arbitrary-precision integer, split across basic / calculation / comparison / conversion files) and `Rational`, a reduced-fraction type holding a numerator/denominator pair of `BigInt`s. This allows computation without overflow or rounding error.

The token delimiter is configurable (space and tab by default), with validation that the delimiter does not collide with digits or operators.

### ex02: Ford-Johnson (Merge-Insertion) Sort with a Multi-Algorithm Comparison Benchmark (PmergeMe)

The Ford-Johnson (merge-insertion sort) implementation is broken down into the following five stages (`ex02/detail/`):

1. `1_cmp_neighbor.hpp` — pairwise comparison of neighboring elements
2. `2_reorder_pairs.hpp` — reordering of pairs
3. `3_create_main_chain.hpp` — construction of the main chain
4. `4_insert_into_main_chain.hpp` — insertion guided by the Jacobsthal sequence
5. `5_write_back.hpp` — writing the result back

`ex02/benchmark/` contains 22 additional sorting algorithms implemented for comparison, measuring both comparison count and execution time.

quick / merge / heap / insertion / bubble / selection / binary insertion / intro / tim / shell / odd-even / comb / cycle / gnome / shaker / patience / tree / tournament / ternary split quick / cartesian tree sort

Supporting data structures used for comparison include `BinarySearchTree` / `Cartesian` (for tree sort) / `ImplicitTreap`.

#### Benchmark Results (measured)

Excerpt from running `ex02/benchmark/benchmark.cpp` on 50 trials of random `int` sequences of 3,000 elements (range 1–10,000,000).

| Algorithm | Max Cmp | Avg Cmp | Max Time (us) | Avg Time (us) |
|---|---:|---:|---:|---:|
| Information-theoretic lower bound ⌈log2(n!)⌉ | 30,332 | 30,332.00 | - | - |
| Ford-Johnson F(n) (worst-case comparison upper bound for a correct implementation) | 30,546 | 30,546.00 | - | - |
| **PmergeMe (this implementation)** | 30,402 | 30,379.88 | 7,096 | 6,457.28 |
| std::sort | 46,113 | 41,689.26 | 255 | 187.48 |
| std::stable_sort | 33,063 | 32,869.20 | 279 | 208.86 |
| Merge Sort | 31,013 | 30,927.04 | 656 | 491.10 |
| Binary Insertion Sort | 30,545 | 30,501.86 | 535 | 460.84 |
| Tim Sort | 30,896 | 30,842.56 | 325 | 248.00 |
| Bubble Sort | 4,498,455 | 4,495,535.42 | 22,542 | 18,973.82 |
| Cycle Sort | 13,481,040 | 13,452,679.06 | 17,362 | 15,008.42 |

PmergeMe's average comparison count (30,379.88) stays under F(n) (30,546), the worst-case upper bound for a correct implementation, confirming the implementation behaves as the theory predicts. It also sits close to the information-theoretic lower bound (30,332). Execution time, however, is about 35x slower than `std::sort` (average 187us), which shows numerically that comparison count and execution time are separate metrics — the gap comes from Ford-Johnson's overhead and constant factors.

### Shared Infrastructure: toolbox/

Code shared across all three exercises.

- `StepMark`: a logger with 8 severity levels (TRACE–FATAL). Writes execution logs to `ex00.log` / `ex01.log` / `ex02.log` for each exercise
- `string`: utilities equivalent to `to_string` / `stoi` / `stod` for a C++98 environment
- `math`: math utilities such as `gcd`
- `color`: colored terminal output

## Build & Run

Compiled against C++98 with `-Wall -Wextra -Werror`.

```sh
cd ex00 && make && ./btc input.txt
cd ex01 && make && ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
cd ex02 && make && ./PmergeMe 3 5 9 7 4
```
