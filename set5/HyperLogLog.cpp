#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <functional>
#include <string>

typedef uint32_t uint;

class HyperLogLog {
public:
    int b;
    int m;
    double alph;
    std::vector<uint8_t> reg;
    std::function<uint(const std::string&)> foo;

    int cnt_zeros(uint x, int maxb) {
        if (x == 0) return maxb + 1;
        int count = 0;
        while (count < maxb && ((x >> (maxb - 1 - count)) & 1) == 0) {
            ++count;
        }
        return count + 1;
    }

    HyperLogLog(int bits = 12, std::function<uint(const std::string&)> hf = nullptr) : b(bits) {
        m = 1 << b;
        reg.resize(m, 0);

        if (m == 16) alph = 0.673;
        else if (m == 32) alph = 0.697;
        else if (m == 64) alph = 0.709;
        else alph = 0.7213 / (1.0 + 1.079 / m);

        if (!hf) {
            HashFuncGen h;
            foo = [h](const std::string& s) { return h.murmur_hash(s); };
        } else {
            foo = hf;
        }
    }

    void add(const std::string& s) {
        uint el = foo(s);
        uint i = el >> (32 - b);
        uint val = el & ((1u << (32 - b)) - 1);
        int zeroKol = cnt_zeros(val, 32 - b);

        if (zeroKol > reg[i]) {
            reg[i] = static_cast<uint8_t>(zeroKol);
        }
    }

    double e() const {
        double sum = 0.0;
        for (uint8_t r : reg) {
            sum += 1.0 / std::pow(2.0, r);
        }

        double est = alph * m * m / sum;

        if (est <= 2.5 * m) {
            int z = std::count(reg.begin(), reg.end(), 0);
            if (z) {
                est = m * std::log(static_cast<double>(m) / z);
            }
        }
        if (est > (1.0 / 30.0) * (1ULL << 32)) {
            est = -(1ULL << 32) * std::log(1.0 - est / (1ULL << 32));
        }

        return est;
    }

    void clear() {
        std::fill(reg.begin(), reg.end(), 0);
    }

    static size_t cnt_unique(const std::vector<std::string>& data) {
        std::unordered_set<std::string> s(data.begin(), data.end());
        return s.size();
    }
};