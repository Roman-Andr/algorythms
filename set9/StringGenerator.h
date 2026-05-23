#pragma once

#include <random>
#include <string>
#include <vector>

class StringGenerator {
public:
    static constexpr int MIN_LEN = 10;
    static constexpr int MAX_LEN = 200;
    static constexpr int MAX_N = 3000;

    explicit StringGenerator(unsigned seed = 123456789u);

    const std::string& alphabet() const;

    std::vector<std::string> randomArray(int n);
    std::vector<std::string> randomArrayWithCommonPrefix(int n, int prefixLen);

    std::vector<std::string> reversedSortedFrom(const std::vector<std::string>& base) const;
    std::vector<std::string> almostSortedFrom(const std::vector<std::string>& base,
                                               int swapsCount,
                                               unsigned seedOffset = 0) const;

private:
    std::string alphabet_;
    mutable std::mt19937 rng_;

    std::string randomString();
};
