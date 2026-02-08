#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <fstream>
#include <cmath>
#include "RandomStreamGen.cpp"
#include "HashFuncGen.cpp"
#include "HyperLogLog.cpp"

class Analyzer {
private:
    RandomStreamGen gen;

public:
    void test_choose_b(size_t stream_size = 100000, size_t uniq = 50000) {
        std::cout << "Analysis for choosing b:\n";
        std::vector<int> bs = {4, 6, 8, 10, 12, 14};
        auto stream = gen.gen_with_uniq(stream_size, uniq);
        size_t exact = HyperLogLog::cnt_unique(stream);

        for (int bb : bs) {
            HyperLogLog hll(bb);
            for (const auto& s : stream) {
                hll.add(s);
            }
            double est = hll.e();
            double rel_err = std::fabs(est - exact) / exact * 100;
            double th_err = 1.04 / std::sqrt(1 << bb) * 100;
            std::cout << "b=" << bb << ", m=" << (1 << bb) << ", exact=" << exact 
                      << ", est=" << est << ", rel_err=" << rel_err << "%, th_err=" << th_err << "%\n";
        }
    }

    void test_variance(int bb = 12, int trials = 100, size_t stream_size = 50000, size_t uniq = 25000) {
        std::cout << "\nVariance analysis (b=" << bb << "):\n";
        std::vector<double> rel_errors;
        for (int t = 0; t < trials; ++t) {
            RandomStreamGen gen_t(t);
            auto stream = gen_t.gen_with_uniq(stream_size, uniq);
            HyperLogLog hll(bb);
            for (const auto& s : stream) {
                hll.add(s);
            }
            double est = hll.e();
            double rel_err = std::fabs(est - uniq) / uniq * 100;
            rel_errors.push_back(rel_err);
        }

        double mean_err = 0.0;
        for (double e : rel_errors) mean_err += e;
        mean_err /= trials;

        double var = 0.0;
        for (double e : rel_errors) var += (e - mean_err) * (e - mean_err);
        var /= trials;
        double std_dev = std::sqrt(var);
        double th_std = 1.04 / std::sqrt(1 << bb) * 100;

        std::cout << "Mean rel error: " << mean_err << "%\n";
        std::cout << "Std dev of rel error: " << std_dev << "%\n";
        std::cout << "Theoretical std: " << th_std << "%\n";
    }

    void generate_results_csv(int num_streams = 10, size_t stream_size = 100000, size_t uniq = 50000, int bb = 12) {
        std::vector<double> percents = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        std::ofstream csv("results.csv");
        csv << "percent,mean_exact,mean_est,std_est\n";

        for (double perc : percents) {
            double sum_exact = 0.0, sum_est = 0.0, sum_sq_est = 0.0;
            for (int s = 0; s < num_streams; ++s) {
                RandomStreamGen gen_s(s);
                auto stream = gen_s.gen_with_uniq(stream_size, uniq);
                size_t end = static_cast<size_t>(stream.size() * (perc / 100.0));
                std::vector<std::string> part(stream.begin(), stream.begin() + end);
                size_t exact = HyperLogLog::cnt_unique(part);
                HyperLogLog hll(bb);
                for (const auto& str : part) {
                    hll.add(str);
                }
                double est = hll.e();
                sum_exact += exact;
                sum_est += est;
                sum_sq_est += est * est;
            }
            double mean_exact = sum_exact / num_streams;
            double mean_est = sum_est / num_streams;
            double var_est = (sum_sq_est / num_streams) - (mean_est * mean_est);
            double std_est = std::sqrt(var_est);

            csv << perc << "," << mean_exact << "," << mean_est << "," << std_est << "\n";
        }
    }
};

int main() {
    Analyzer a;
    a.test_choose_b();
    a.test_variance();
    a.generate_results_csv();

    RandomStreamGen gen(0);
    auto stream = gen.gen_with_uniq(100000, 50000);
    std::ofstream stream_file("stream_0.txt");
    for (const auto& s : stream) {
        stream_file << s << "\n";
    }

    HashFuncGen hasher;
    hasher.test_uniformity(1000000, 1024);

    return 0;
}