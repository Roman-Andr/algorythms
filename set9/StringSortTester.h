#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct SortStats {
    long long charComparisons = 0;
};

struct BenchmarkResult {
    std::string datasetType;
    int n = 0;
    std::string algorithm;
    double avgTimeUs = 0.0;
    double avgCharComparisons = 0.0;
    int runs = 0;
    bool sortedOk = false;
};

class StringSortTester {
public:
    using Array = std::vector<std::string>;

    static const std::vector<std::string>& algorithmNames();

    BenchmarkResult benchmark(const std::string& datasetType,
                              const std::string& algorithm,
                              const Array& input,
                              int runs) const;

    void sortByName(const std::string& algorithm, Array& a, SortStats& stats) const;

    static bool isSorted(const Array& a);

private:
    static constexpr int ASCII_RANGE = 256;
    static constexpr int RADIX_KEYS = ASCII_RANGE + 1; // sentinel + 256 unsigned char values
    static constexpr int ALPHABET_POWER = 74;

    static int charKey(const std::string& s, int depth, SortStats& stats);
    static int charKeyNoCount(const std::string& s, int depth);
    static int countedLexCompare(const std::string& a, const std::string& b, SortStats& stats, int startDepth = 0);
    static int lcp(const std::string& a, const std::string& b, SortStats& stats, int startDepth = 0);

    static void standardQuickSort(Array& a, int lo, int hi, SortStats& stats);
    static void standardMergeSort(Array& a, Array& aux, int lo, int hi, SortStats& stats);
    static void standardMerge(Array& a, Array& aux, int lo, int mid, int hi, SortStats& stats);

    static void stringQuickSort(Array& a, int lo, int hi, int depth, SortStats& stats);

    static void stringMergeSort(Array& a, Array& aux, int lo, int hi, SortStats& stats);
    static void stringMerge(Array& a, Array& aux, int lo, int mid, int hi, SortStats& stats);

    static void msdRadixSort(Array& a, Array& aux, int lo, int hi, int depth, SortStats& stats, bool switchToQuick);
};
