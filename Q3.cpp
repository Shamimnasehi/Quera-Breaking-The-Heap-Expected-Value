/*
 * Q3 - Breaking the Heap (Expected Values After a Random Heap Insertion)
 *
 * A max-heap of n songs is stored in a 1-indexed array: the popularity of song
 * i is never smaller than the popularity of songs 2i and 2i+1. A new song is
 * added whose popularity X is uniformly distributed over the integers in
 * [L, H]. It is appended at index n+1 and sifted up: while it is not at index
 * 1 and its popularity is strictly greater than that of its parent (index
 * floor(k/2)), the two are swapped.
 *
 * For every index 1..n+1 print the expected popularity of the song that ends
 * up there. The answer may be fractional; an absolute error up to 0.001 is
 * accepted.
 *
 * Key observations
 *   1. The new song only moves along the path from index n+1 up to the root.
 *      Every other index keeps its original value, so its expectation is
 *      simply a[i].
 *   2. Going up that path the original values never decrease (heap property).
 *      So the new song climbs past exactly the ancestors whose value is
 *      strictly smaller than X, and stops right below the first ancestor whose
 *      value is >= X. (Ties never matter: swapping equal values changes
 *      nothing.)
 *   3. Let p_0 = n+1 (the new slot), p_1 its parent, ..., p_last = 1 (the
 *      root). The final value at p_i is
 *          X            if a[p_i] < X <= a[p_{i+1}]   (X stops here)
 *          a[p_{i+1}]   if X > a[p_{i+1}]             (parent moved down)
 *          a[p_i]       if X <= a[p_i]                (X never reached here)
 *      with the obvious conventions at both ends of the path (p_0 holds no
 *      original value, the root has no parent).
 *      Each case covers a contiguous range of X, so its contribution to the
 *      expectation is a count or a sum of consecutive integers: O(1) per node.
 *
 * Precision: sums reach about 10^30, so __int128 is used. The expectation is
 * kept as an exact fraction (numerator / total); its integer part and six
 * decimal digits are produced with integer arithmetic, so there is no
 * floating-point error at all (the printed value is rounded, error <= 5e-7).
 *
 * Complexity: O(n) time (reading the input dominates), O(n) memory.
 */

#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;
typedef __int128 i128;

static ll L, H;     // range of the new song's popularity, both inclusive
static i128 total;  // number of possible values: H - L + 1

// Number of integers x in [L, H] with x <= v.
static ll countAtMost(ll v) {
    if (v < L) return 0;
    if (v > H) v = H;
    return v - L + 1;
}

// Sum of the integers x in [L, H] with x <= v.
static i128 sumAtMost(ll v) {
    if (v < L) return 0;
    if (v > H) v = H;
    return (i128)(L + v) * (v - L + 1) / 2;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    vector<ll> a(n + 2, 0);  // 1-indexed, a[n + 1] is the empty slot
    for (int i = 1; i <= n; i++) cin >> a[i];
    cin >> L >> H;
    total = (i128)H - L + 1;

    // path[0] = n + 1 (new slot), path[1] = its parent, ..., path.back() = 1.
    vector<int> path;
    for (int k = n + 1; k >= 1; k /= 2) path.push_back(k);
    int last = static_cast<int>(path.size()) - 1;

    // expected value at index k = numerator[k] / total.
    // Indices off the path keep their original value.
    vector<i128> numerator(n + 2);
    for (int k = 1; k <= n + 1; k++) numerator[k] = (i128)a[k] * total;

    for (int i = 0; i <= last; i++) {
        bool hasParent = i < last;  // some ancestor sits above this node
        bool hasOwn = i > 0;        // this node held an original value

        // X stops exactly here: own < X <= parent (open-ended at both ends).
        i128 num = sumAtMost(hasParent ? a[path[i + 1]] : H);
        if (hasOwn) num -= sumAtMost(a[path[i]]);

        // X > parent: the parent's value moved down into this node.
        if (hasParent) {
            ll parentValue = a[path[i + 1]];
            num += (i128)parentValue * (total - countAtMost(parentValue));
        }

        // X <= own value: X never got this far, the node is unchanged.
        if (hasOwn) {
            ll ownValue = a[path[i]];
            num += (i128)ownValue * countAtMost(ownValue);
        }

        numerator[path[i]] = num;
    }

    for (int k = 1; k <= n + 1; k++) {
        i128 whole = numerator[k] / total;
        i128 rest = numerator[k] % total;

        // Round to 6 decimals: floor(rest / total * 10^6 + 1/2), exactly.
        ll micro = static_cast<ll>((rest * 2000000 + total) / (2 * total));
        if (micro == 1000000) {  // rounded up to the next whole number
            whole++;
            micro = 0;
        }
        printf("%lld.%06lld%c", static_cast<ll>(whole), micro,
               k == n + 1 ? '\n' : ' ');
    }

    return 0;
}
