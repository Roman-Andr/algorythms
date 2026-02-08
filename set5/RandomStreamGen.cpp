// RandomStreamGen.cpp
#include <vector>
#include <string>
#include <random>
#include <unordered_set>

class RandomStreamGen {
public:
    std::mt19937 rnd;
    std::uniform_int_distribution<int> ch;
    std::uniform_int_distribution<int> len;
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";

    RandomStreamGen(unsigned int seed = 13) : rnd(seed), ch(0, 62), len(1, 30) {} // Changed min len to 1 as per task

    std::string generate_str() {
        int l = len(rnd);
        std::string res;
        res.reserve(l);
        for (int i = 0; i < l; ++i) {
            res += alphabet[ch(rnd)];
        }
        return res;
    }

    std::vector<std::string> generate(size_t n) {
        std::vector<std::string> s;
        s.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            s.push_back(generate_str());
        }
        return s;
    }

    std::vector<std::string> gen_with_uniq(size_t n, size_t nUniq) {
        std::vector<std::string> uniqStr;
        std::unordered_set<std::string> uniqSet;
        std::vector<std::string> stream;
        stream.reserve(n);

        while (uniqStr.size() < nUniq) {
            std::string str = generate_str();
            if (uniqSet.insert(str).second) {
                uniqStr.push_back(str);
            }
        }

        stream.insert(stream.end(), uniqStr.begin(), uniqStr.end());
        std::uniform_int_distribution<size_t> v(0, nUniq - 1);
        while (stream.size() < n) {
            size_t i = v(rnd);
            stream.push_back(uniqStr[i]);
        }
        return stream;
    }

    std::vector<std::vector<std::string>> split(const std::vector<std::string>& s, const std::vector<double>& percent) {
        std::vector<std::vector<std::string>> v;
        v.reserve(percent.size());
        for (double perc : percent) {
            size_t end = static_cast<size_t>(s.size() * (perc / 100.0));
            v.emplace_back(s.begin(), s.begin() + end);
        }
        return v;
    }
};