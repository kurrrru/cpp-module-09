# cpp-module-09

C++で、複数暦法対応の日付変換によるビットコイン価格換算・任意精度有理数による逆ポーランド記法(RPN)電卓・Ford-Johnsonアルゴリズムによる整数ソートと多アルゴリズム比較ベンチマークを実装したリポジトリです（42 Cursusのカリキュラムがベースになっています）。

## 構成

- ex00/ Bitcoin Exchange — 複数暦法対応の日付変換によるビットコイン価格換算
- ex01/ RPN — 任意精度有理数による逆ポーランド記法演算
- ex02/ PmergeMe — Ford-Johnson (merge-insertion) ソートとソートアルゴリズム比較ベンチマーク
- toolbox/ 3課題共通の基盤（ロガー・文字列・数学ユーティリティ）

### ex00: 複数暦法対応の日付変換によるビットコイン価格換算 (BitcoinExchange)

日付の処理は `ICalendarSystem` という抽象インターフェース（`ex00/calendar_system/ICalendarSystem.hpp`）の上に実装されています。日付とシリアル値（`to_serial_date` / `from_serial_date`）を相互変換する共通インターフェースに対して、以下の暦法を実装しています。

- グレゴリオ暦 (`GregorianCalendar`)
- ユリウス暦 (`JulianCalendar`)
- フランス革命暦 (`FrenchRepublicanCalendar`)
- エチオピア暦 (`EthiopianCalendar`)
- 非先発グレゴリオ暦 (`NonProlepticGregorianCalendar`)

プログラムが使用しているのはグレゴリオ暦です。

### ex01: 任意精度有理数による逆ポーランド記法演算 (RPN)

数値の演算には `BigInt`（任意精度整数、basic / calculation / comparison / conversion の4ファイルに分割）と、それを分子・分母に持つ既約分数型 `Rational` を使用しています。オーバーフローや誤差なしに計算できます。

トークンの区切り文字は可変で（デフォルトは空白とタブ）、区切り文字が数字・演算子と衝突していないかのバリデーションもあります。

### ex02: Ford-Johnson (merge-insertion) ソートとソートアルゴリズム比較ベンチマーク (PmergeMe)

Ford-Johnson (merge-insertion sort) の実装は以下の5段階に分解されています（`ex02/detail/`）。

1. `1_cmp_neighbor.hpp` — 隣接要素のペア比較
2. `2_reorder_pairs.hpp` — ペアの並び替え
3. `3_create_main_chain.hpp` — メインチェーンの構築
4. `4_insert_into_main_chain.hpp` — Jacobsthal数列に基づく挿入
5. `5_write_back.hpp` — 結果の書き戻し

`ex02/benchmark/` には比較用に22種類のソートアルゴリズムを実装しており、比較回数・実行時間の両方を計測できます。

quick / merge / heap / insertion / bubble / selection / binary insertion / intro / tim / shell / odd-even / comb / cycle / gnome / shaker / patience / tree / tournament / ternary split quick / cartesian tree sort

比較用のデータ構造として `BinarySearchTree` / `Cartesian`（tree sort用） / `ImplicitTreap` があります。

#### ベンチマーク結果（実測）

3000要素・50試行・`int` 型のランダム列（1〜10,000,000）を用いて `ex02/benchmark/benchmark.cpp` を実行した結果の抜粋です。

| アルゴリズム | Max Cmp | Avg Cmp | Max Time (us) | Avg Time (us) |
|---|---:|---:|---:|---:|
| 理論下限 ⌈log2(n!)⌉ | 30,332 | 30,332.00 | - | - |
| Ford-Johnson F(n)（正しく実装した場合の最悪比較回数の上界） | 30,546 | 30,546.00 | - | - |
| **PmergeMe（自実装）** | 30,402 | 30,379.88 | 7,096 | 6,457.28 |
| std::sort | 46,113 | 41,689.26 | 255 | 187.48 |
| std::stable_sort | 33,063 | 32,869.20 | 279 | 208.86 |
| Merge Sort | 31,013 | 30,927.04 | 656 | 491.10 |
| Binary Insertion Sort | 30,545 | 30,501.86 | 535 | 460.84 |
| Tim Sort | 30,896 | 30,842.56 | 325 | 248.00 |
| Bubble Sort | 4,498,455 | 4,495,535.42 | 22,542 | 18,973.82 |
| Cycle Sort | 13,481,040 | 13,452,679.06 | 17,362 | 15,008.42 |

PmergeMe の平均比較回数（30,379.88）は、正しく実装した場合の最悪比較回数の上界であるF(n)（30,546）を下回っており、実装が理論通りに機能していることが確認できます。さらに情報理論的な下限（30,332）にもほぼ張り付いています。一方で実行時間は `std::sort`（平均187us）の約35倍かかっており、比較回数と実行時間は別の指標であることが数値から分かります（Ford-Johnsonのオーバーヘッドと定数倍の大きさが要因）。

### 共通基盤: toolbox/

3つの課題から使われる共有コードです。

- `StepMark`：8段階（TRACE〜FATAL）のログレベルを持つロガー。各課題の実行ログを `ex00.log` / `ex01.log` / `ex02.log` に出力
- `string`：C++98環境向けの `to_string` / `stoi` / `stod` 相当のユーティリティ
- `math`：`gcd` などの数学ユーティリティ
- `color`：ターミナル出力の色付け

## ビルド・実行方法

C++98準拠、`-Wall -Wextra -Werror` でコンパイルしています。

```sh
cd ex00 && make && ./btc input.txt
cd ex01 && make && ./RPN "8 9 * 9 - 9 - 9 - 4 - 1 +"
cd ex02 && make && ./PmergeMe 3 5 9 7 4
