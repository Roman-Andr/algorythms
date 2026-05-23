#include "StringSortTester.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>

const std::vector<std::string>& StringSortTester::algorithmNames() {
    static const std::vector<std::string> names = {
        "standard_quicksort",
        "standard_mergesort",
        "string_quicksort",
        "string_mergesort_lcp",
        "msd_radix_sort",
        "msd_radix_quick_sort"
    };
    return names;
}

bool StringSortTester::isSorted(const Array& a) {
    return std::is_sorted(a.begin(), a.end());
}

int StringSortTester::charKey(const std::string& s, int depth, SortStats& stats) {
    ++stats.charComparisons; // for radix algorithms this is a character inspection
    if (depth >= static_cast<int>(s.size())) {
        return 0;
    }
    return static_cast<unsigned char>(s[depth]) + 1;
}

int StringSortTester::charKeyNoCount(const std::string& s, int depth) {
    if (depth >= static_cast<int>(s.size())) {
        return 0;
    }
    return static_cast<unsigned char>(s[depth]) + 1;
}

int StringSortTester::countedLexCompare(const std::string& a,
                                        const std::string& b,
                                        SortStats& stats,
                                        int startDepth) {
    int i = std::max(0, startDepth);
    while (i < static_cast<int>(a.size()) && i < static_cast<int>(b.size())) {
        ++stats.charComparisons;
        unsigned char ca = static_cast<unsigned char>(a[i]);
        unsigned char cb = static_cast<unsigned char>(b[i]);
        if (ca < cb) return -1;
        if (ca > cb) return 1;
        ++i;
    }
    if (a.size() == b.size()) return 0;
    return a.size() < b.size() ? -1 : 1;
}

int StringSortTester::lcp(const std::string& a, const std::string& b, SortStats& stats, int startDepth) {
    int i = std::max(0, startDepth);
    while (i < static_cast<int>(a.size()) && i < static_cast<int>(b.size())) {
        ++stats.charComparisons;
        if (a[i] != b[i]) {
            break;
        }
        ++i;
    }
    return i;
}

void StringSortTester::standardQuickSort(Array& a, int lo, int hi, SortStats& stats) {
    if (lo >= hi) return;

    int mid = lo + (hi - lo) / 2; // same pivot-position rule as in ternary string quicksort
    std::swap(a[lo], a[mid]);
    const std::string pivot = a[lo];

    int lt = lo;
    int i = lo + 1;
    int gt = hi;

    while (i <= gt) {
        int cmp = countedLexCompare(a[i], pivot, stats);
        if (cmp < 0) {
            std::swap(a[lt++], a[i++]);
        } else if (cmp > 0) {
            std::swap(a[i], a[gt--]);
        } else {
            ++i;
        }
    }

    standardQuickSort(a, lo, lt - 1, stats);
    standardQuickSort(a, gt + 1, hi, stats);
}

void StringSortTester::standardMerge(Array& a, Array& aux, int lo, int mid, int hi, SortStats& stats) {
    int i = lo;
    int j = mid + 1;
    int k = lo;

    while (i <= mid && j <= hi) {
        if (countedLexCompare(a[i], a[j], stats) <= 0) {
            aux[k++] = std::move(a[i++]);
        } else {
            aux[k++] = std::move(a[j++]);
        }
    }
    while (i <= mid) aux[k++] = std::move(a[i++]);
    while (j <= hi) aux[k++] = std::move(a[j++]);
    for (int p = lo; p <= hi; ++p) {
        a[p] = std::move(aux[p]);
    }
}

void StringSortTester::standardMergeSort(Array& a, Array& aux, int lo, int hi, SortStats& stats) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    standardMergeSort(a, aux, lo, mid, stats);
    standardMergeSort(a, aux, mid + 1, hi, stats);
    standardMerge(a, aux, lo, mid, hi, stats);
}

void StringSortTester::stringQuickSort(Array& a, int lo, int hi, int depth, SortStats& stats) {
    if (lo >= hi) return;

    int mid = lo + (hi - lo) / 2; // same pivot-position rule as standard quicksort
    std::swap(a[lo], a[mid]);

    int lt = lo;
    int gt = hi;
    int pivot = charKey(a[lo], depth, stats);
    int i = lo + 1;

    while (i <= gt) {
        int current = charKey(a[i], depth, stats);
        ++stats.charComparisons;
        if (current < pivot) {
            std::swap(a[lt++], a[i++]);
        } else {
            ++stats.charComparisons;
            if (current > pivot) {
                std::swap(a[i], a[gt--]);
            } else {
                ++i;
            }
        }
    }

    stringQuickSort(a, lo, lt - 1, depth, stats);
    if (pivot != 0) {
        stringQuickSort(a, lt, gt, depth + 1, stats);
    }
    stringQuickSort(a, gt + 1, hi, depth, stats);
}

void StringSortTester::stringMerge(Array& a, Array& aux, int lo, int mid, int hi, SortStats& stats) {
    int i = lo;
    int j = mid + 1;
    int k = lo;

    bool hasLast = false;
    std::string last;
    int lcpLeft = 0;
    int lcpRight = 0;

    auto refresh = [&]() {
        if (!hasLast) {
            lcpLeft = lcpRight = 0;
            return;
        }
        if (i <= mid) lcpLeft = lcp(last, a[i], stats);
        if (j <= hi) lcpRight = lcp(last, a[j], stats);
    };

    while (i <= mid && j <= hi) {
        int takeLeft;
        if (hasLast && lcpLeft > lcpRight) {
            takeLeft = 1;
        } else if (hasLast && lcpLeft < lcpRight) {
            takeLeft = 0;
        } else {
            int start = hasLast ? lcpLeft : 0;
            takeLeft = countedLexCompare(a[i], a[j], stats, start) <= 0;
        }

        if (takeLeft) {
            aux[k++] = std::move(a[i++]);
        } else {
            aux[k++] = std::move(a[j++]);
        }
        last = aux[k - 1];
        hasLast = true;
        refresh();
    }

    while (i <= mid) aux[k++] = std::move(a[i++]);
    while (j <= hi) aux[k++] = std::move(a[j++]);
    for (int p = lo; p <= hi; ++p) {
        a[p] = std::move(aux[p]);
    }
}

void StringSortTester::stringMergeSort(Array& a, Array& aux, int lo, int hi, SortStats& stats) {
    if (lo >= hi) return;
    int mid = lo + (hi - lo) / 2;
    stringMergeSort(a, aux, lo, mid, stats);
    stringMergeSort(a, aux, mid + 1, hi, stats);
    stringMerge(a, aux, lo, mid, hi, stats);
}

void StringSortTester::msdRadixSort(Array& a,
                                    Array& aux,
                                    int lo,
                                    int hi,
                                    int depth,
                                    SortStats& stats,
                                    bool switchToQuick) {
    if (lo >= hi) return;
    if (switchToQuick && hi - lo + 1 < ALPHABET_POWER) {
        stringQuickSort(a, lo, hi, depth, stats);
        return;
    }

    int count[RADIX_KEYS] = {0};
    for (int i = lo; i <= hi; ++i) {
        ++count[charKey(a[i], depth, stats)];
    }

    int start[RADIX_KEYS + 1] = {0};
    for (int key = 0; key < RADIX_KEYS; ++key) {
        start[key + 1] = start[key] + count[key];
    }

    int next[RADIX_KEYS];
    for (int key = 0; key < RADIX_KEYS; ++key) {
        next[key] = start[key];
    }

    for (int i = lo; i <= hi; ++i) {
        int key = charKeyNoCount(a[i], depth);
        aux[lo + next[key]++] = std::move(a[i]);
    }
    for (int i = lo; i <= hi; ++i) {
        a[i] = std::move(aux[i]);
    }

    // key 0 is the end-of-string sentinel; strings in it are already final.
    for (int key = 1; key < RADIX_KEYS; ++key) {
        int bucketLo = lo + start[key];
        int bucketHi = lo + start[key + 1] - 1;
        if (bucketLo < bucketHi) {
            msdRadixSort(a, aux, bucketLo, bucketHi, depth + 1, stats, switchToQuick);
        }
    }
}

void StringSortTester::sortByName(const std::string& algorithm, Array& a, SortStats& stats) const {
    if (a.empty()) return;

    if (algorithm == "standard_quicksort") {
        standardQuickSort(a, 0, static_cast<int>(a.size()) - 1, stats);
    } else if (algorithm == "standard_mergesort") {
        Array aux(a.size());
        standardMergeSort(a, aux, 0, static_cast<int>(a.size()) - 1, stats);
    } else if (algorithm == "string_quicksort") {
        stringQuickSort(a, 0, static_cast<int>(a.size()) - 1, 0, stats);
    } else if (algorithm == "string_mergesort_lcp") {
        Array aux(a.size());
        stringMergeSort(a, aux, 0, static_cast<int>(a.size()) - 1, stats);
    } else if (algorithm == "msd_radix_sort") {
        Array aux(a.size());
        msdRadixSort(a, aux, 0, static_cast<int>(a.size()) - 1, 0, stats, false);
    } else if (algorithm == "msd_radix_quick_sort") {
        Array aux(a.size());
        msdRadixSort(a, aux, 0, static_cast<int>(a.size()) - 1, 0, stats, true);
    } else {
        throw std::invalid_argument("Unknown sorting algorithm: " + algorithm);
    }
}

BenchmarkResult StringSortTester::benchmark(const std::string& datasetType,
                                            const std::string& algorithm,
                                            const Array& input,
                                            int runs) const {
    BenchmarkResult result;
    result.datasetType = datasetType;
    result.n = static_cast<int>(input.size());
    result.algorithm = algorithm;
    result.runs = runs;

    long long totalComparisons = 0;
    long long totalTimeNs = 0;
    bool ok = true;

    for (int run = 0; run < runs; ++run) {
        Array data = input;
        SortStats stats;

        auto start = std::chrono::steady_clock::now();
        sortByName(algorithm, data, stats);
        auto finish = std::chrono::steady_clock::now();

        totalTimeNs += std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
        totalComparisons += stats.charComparisons;
        ok = ok && isSorted(data);
    }

    result.avgTimeUs = static_cast<double>(totalTimeNs) / runs / 1000.0;
    result.avgCharComparisons = static_cast<double>(totalComparisons) / runs;
    result.sortedOk = ok;
    return result;
}
