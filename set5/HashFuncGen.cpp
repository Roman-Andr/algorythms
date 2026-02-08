#include <string>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <random>

typedef uint32_t uint;

class HashFuncGen {
public:
    uint seed;
    HashFuncGen(uint s = 13) : seed(s) {}

    uint murmur_hash(const std::string& s) const {
        uint c1 = 0xcc9e2d51;
        uint c2 = 0x1b873593;
        uint r1 = 15;
        uint r2 = 13;
        uint m = 5;
        uint n = 0xe6546b64;
        uint h = seed;
        size_t l = s.length();

        for (size_t i = 0; i < l; i += 4) {
            uint k = 0;
            for (int j = 0; j < 4 && (i + j) < l; ++j) {
                k |= (static_cast<uint>(s[i + j]) << (8 * j));
            }
            k *= c1;
            k = (k << r1) | (k >> (32 - r1));
            k *= c2;
            h ^= k;
            h = (h << r2) | (h >> (32 - r2));
            h = h * m + n;
        }

        h ^= l;
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    void test_uniformity(size_t num_samples, size_t num_buckets) {
        std::mt19937 gen(42);
        std::uniform_int_distribution<int> len_dist(1, 30);
        std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
        std::uniform_int_distribution<int> char_dist(0, 62);

        std::vector<int> buckets(num_buckets, 0);
        for (size_t i = 0; i < num_samples; ++i) {
            int len = len_dist(gen);
            std::string s(len, ' ');
            for (int j = 0; j < len; ++j) {
                s[j] = chars[char_dist(gen)];
            }
            uint h = murmur_hash(s);
            buckets[h % num_buckets]++;
        }

        int expected = static_cast<int>(num_samples / num_buckets);
        std::cout << "Uniformity: min " << *std::min_element(buckets.begin(), buckets.end()) 
                  << ", max " << *std::max_element(buckets.begin(), buckets.end()) 
                  << ", expected " << expected << std::endl;
    }
};