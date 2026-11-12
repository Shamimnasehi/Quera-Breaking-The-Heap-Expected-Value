# Breaking the Heap (Expected Values After a Random Heap Insertion)

Algorithm Design - Homework 1, Problem 3 (Heap & Greedy algorithms)

- **Time limit:** 1 second
- **Memory limit:** 256 MB

## Problem

A band is planning the song list for a concert. The list holds `n` old songs and
one new song. The songs are stored in an array (1-indexed) that is a
**max-heap** by popularity: the popularity of song `i` is never smaller than the
popularity of songs `2i` and `2i + 1`.

The new song is not finished yet. Its popularity `X` is predicted to be an
integer in `[L, H]`, and every value in that range is **equally likely**.

Once the song is released it is added to the list like a heap insertion:

1. Append it at the end of the array (index `n + 1`).
2. While the song is at index `k > 1` and its popularity is strictly greater
   than the popularity of the song at index `floor(k / 2)`, swap the two.

Print, for every position of the final list, the **expected value** of the
popularity of the song that ends up there.

### Input

- First line: the integer `n`, the number of old songs.
- Second line: `n` integers, the popularity `fav_i` of song `i`.
- Third line: two integers `L` and `H`.

Constraints:

```
n <= 2 * 10^5
1 <= fav_i <= 10^15
1 <= L <= H <= 10^15
```

### Output

`n + 1` numbers, the `i`-th being the expected popularity at position `i` of the
list after the insertion. An answer is accepted if it differs from the expected
output by at most `0.001`.

### Example

Input:

```
3
80 20 50
10 90
```

Output:

```
80.679012 50.000000 50.000000 19.320988
```

## Solution

The new song can only move along one path, so almost the whole array is
unaffected.

1. **Only one path changes.** The new song climbs from index `n + 1` towards the
   root. Every index off that path keeps its original value, so its expected
   value is just `a[i]`.
2. **Values on the path are sorted.** Because of the heap property, going up
   the path the original values never decrease. Let `p_0 = n + 1` be the new
   slot, `p_1` its parent, ..., `p_last = 1` the root. The new song climbs past
   exactly the ancestors whose value is strictly smaller than `X`.
   (Swapping equal values changes nothing, so ties are harmless.)
3. **Value at each path node.** For node `p_i`:

   | Case                      | Final value at `p_i` | Why                        |
   |---------------------------|----------------------|----------------------------|
   | `a[p_i] < X <= a[p_{i+1}]` | `X`                  | `X` stops here             |
   | `X > a[p_{i+1}]`           | `a[p_{i+1}]`         | the parent moved down      |
   | `X <= a[p_i]`              | `a[p_i]`             | `X` never reached this node |

   (`p_0` has no original value and the root has no parent, so the first and
   last nodes only use the cases that exist.)
4. **O(1) per node.** Each case covers a contiguous range of `X` values, so its
   contribution to the expectation is a *count* or a *sum of consecutive
   integers*. With `total = H - L + 1`:

   ```
   E[p_i] = ( sum of X in (a[p_i], a[p_{i+1}]]
              + a[p_{i+1}] * #{X > a[p_{i+1}]}
              + a[p_i]     * #{X <= a[p_i]} ) / total
   ```

### Precision

Sums reach about `10^30`, which does not fit in 64 bits, so `__int128` is used.
Each expectation is kept as an exact fraction and the integer part plus six
decimal digits are computed with integer arithmetic only, so there is no
floating-point error (printed values are correctly rounded, error `<= 5e-7`).

**Complexity:** `O(n)` time (reading the input dominates, the path has only
`O(log n)` nodes) and `O(n)` memory.

## Testing

The solution was checked against:

- a literal simulation that tries **every** value of `X` and does the sift-up
  exactly as in the statement (small inputs, including ties and `L == H`),
- an independent exact-fraction reference that also handles huge ranges
  (values up to `10^15`),
- maximum-size inputs (`n = 2 * 10^5`), which run in about 0.07 s.

## Build & Run

```bash
g++ -std=c++17 -O2 Q3.cpp -o Q3
./Q3 < input.txt
```

> Note: `__int128` is a GCC/Clang extension (works with `g++` and `clang++`, not
> with MSVC).

