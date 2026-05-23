#include "StringGenerator.h"
#include "StringSortTester.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
int getIntArg(int argc, char** argv, const std::string& name, int defaultValue) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == name) {
            return std::stoi(argv[i + 1]);
        }
    }
    return defaultValue;
}

std::string getStringArg(int argc, char** argv, const std::string& name, const std::string& defaultValue) {
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == name) {
            return argv[i + 1];
        }
    }
    return defaultValue;
}

void printUsage(const char* program) {
    std::cerr << "Usage: " << program
              << " [--max-n 3000] [--step 100] [--runs 5] [--seed 123456789] [--out results.csv]\n";
}
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        }
    }

    int maxN = getIntArg(argc, argv, "--max-n", 3000);
    int step = getIntArg(argc, argv, "--step", 100);
    int runs = getIntArg(argc, argv, "--runs", 5);
    int seed = getIntArg(argc, argv, "--seed", 123456789);
    std::string outPath = getStringArg(argc, argv, "--out", "results.csv");

    if (maxN <= 0 || maxN > StringGenerator::MAX_N || step <= 0 || runs <= 0) {
        std::cerr << "Invalid arguments. maxN must be in [1, 3000], step/runs must be positive.\n";
        return 1;
    }

    StringGenerator generator(static_cast<unsigned>(seed));
    StringSortTester tester;

    auto randomMax = generator.randomArray(maxN);
    auto reverseMax = generator.reversedSortedFrom(randomMax);
    int swaps = std::max(1, maxN / 20); // 5% pair swaps for the maximum array
    auto almostMax = generator.almostSortedFrom(randomMax, swaps);

    struct Dataset {
        std::string name;
        std::vector<std::string> data;
    };

    std::vector<Dataset> datasets = {
        {"random", randomMax},
        {"reverse_sorted", reverseMax},
        {"almost_sorted", almostMax}
    };

    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "Cannot open output CSV: " << outPath << "\n";
        return 1;
    }

    out << "dataset_type,n,algorithm,avg_time_us,avg_char_comparisons,runs,sorted_ok\n";
    out << std::fixed << std::setprecision(3);

    for (const auto& dataset : datasets) {
        for (int n = step; n <= maxN; n += step) {
            std::vector<std::string> prefix(dataset.data.begin(), dataset.data.begin() + n);
            for (const auto& algorithm : StringSortTester::algorithmNames()) {
                BenchmarkResult r = tester.benchmark(dataset.name, algorithm, prefix, runs);
                out << r.datasetType << ','
                    << r.n << ','
                    << r.algorithm << ','
                    << r.avgTimeUs << ','
                    << r.avgCharComparisons << ','
                    << r.runs << ','
                    << (r.sortedOk ? 1 : 0) << '\n';

                std::cerr << dataset.name << " n=" << n << " " << algorithm
                          << " time_us=" << r.avgTimeUs
                          << " chars=" << r.avgCharComparisons
                          << " ok=" << r.sortedOk << '\n';
            }
        }
    }

    return 0;
}
