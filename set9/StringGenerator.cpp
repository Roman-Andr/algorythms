#include "StringGenerator.h"

#include <algorithm>
#include <stdexcept>

StringGenerator::StringGenerator(unsigned seed)
    : alphabet_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-"),
      rng_(seed) {
    if (alphabet_.size() != 74) {
        throw std::runtime_error("Alphabet size must be exactly 74");
    }
}

const std::string& StringGenerator::alphabet() const {
    return alphabet_;
}

std::string StringGenerator::randomString() {
    std::uniform_int_distribution<int> lenDist(MIN_LEN, MAX_LEN);
    std::uniform_int_distribution<int> charDist(0, static_cast<int>(alphabet_.size()) - 1);

    int len = lenDist(rng_);
    std::string s;
    s.reserve(len);
    for (int i = 0; i < len; ++i) {
        s.push_back(alphabet_[charDist(rng_)]);
    }
    return s;
}

std::vector<std::string> StringGenerator::randomArray(int n) {
    std::vector<std::string> result;
    result.reserve(n);
    for (int i = 0; i < n; ++i) {
        result.push_back(randomString());
    }
    return result;
}

std::vector<std::string> StringGenerator::randomArrayWithCommonPrefix(int n, int prefixLen) {
    std::uniform_int_distribution<int> charDist(0, static_cast<int>(alphabet_.size()) - 1);
    prefixLen = std::max(0, std::min(prefixLen, MAX_LEN - MIN_LEN));

    std::string prefix;
    prefix.reserve(prefixLen);
    for (int i = 0; i < prefixLen; ++i) {
        prefix.push_back(alphabet_[charDist(rng_)]);
    }

    std::vector<std::string> result;
    result.reserve(n);
    for (int i = 0; i < n; ++i) {
        std::string tail = randomString();
        int maxTailLen = MAX_LEN - prefixLen;
        if (static_cast<int>(tail.size()) > maxTailLen) {
            tail.resize(maxTailLen);
        }
        result.push_back(prefix + tail);
    }
    return result;
}

std::vector<std::string> StringGenerator::reversedSortedFrom(const std::vector<std::string>& base) const {
    std::vector<std::string> result = base;
    std::sort(result.begin(), result.end());
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<std::string> StringGenerator::almostSortedFrom(const std::vector<std::string>& base,
                                                            int swapsCount,
                                                            unsigned seedOffset) const {
    std::vector<std::string> result = base;
    std::sort(result.begin(), result.end());
    if (result.empty()) {
        return result;
    }

    std::mt19937 localRng(987654321u + seedOffset);
    std::uniform_int_distribution<int> posDist(0, static_cast<int>(result.size()) - 1);
    for (int i = 0; i < swapsCount; ++i) {
        int first = posDist(localRng);
        int second = posDist(localRng);
        std::swap(result[first], result[second]);
    }
    return result;
}
