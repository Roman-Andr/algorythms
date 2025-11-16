#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using vi = vector<int>;

struct ArrayGenerator {
    std::mt19937 rng;
    int max_n;
    int value_min, value_max;
    vi base_random;
    vi base_sorted;
    vi base_reverse;

    ArrayGenerator(int seed = chrono::high_resolution_clock::now().time_since_epoch().count(),
                   int max_n_ = 100000,
                   int vmin = 0,
                   int vmax = 6000)
        : rng(seed), max_n(max_n_), value_min(vmin), value_max(vmax) {
        base_random.resize(max_n);
        uniform_int_distribution dist(value_min, value_max);
        for (int i = 0; i < max_n; ++i) base_random[i] = dist(rng);

        base_sorted.resize(max_n);
        for (int i = 0; i < max_n; ++i) base_sorted[i] = i;

        base_reverse = base_sorted;
        reverse(base_reverse.begin(), base_reverse.end());
    }

    vi random_prefix(int n) {
        return vi(base_random.begin(), base_random.begin() + n);
    }

    vi reverse_prefix(int n) {
        return vi(base_reverse.begin(), base_reverse.begin() + n);
    }

    vi almost_sorted_prefix(int n, int swaps = 10) {
        static vi tmp;
        tmp.assign(base_sorted.begin(), base_sorted.begin() + n);
        uniform_int_distribution<int> dist(0, n - 1);
        for (int s = 0; s < swaps; ++s) {
            int i = dist(rng);
            int j = dist(rng);
            swap(tmp[i], tmp[j]);
        }
        return tmp;
    }

    vi almost_sorted_prefix_from_full(int n, int swaps = 10) {
        vi tmp(base_sorted.begin(), base_sorted.begin() + max(n, 1));
        uniform_int_distribution<int> dist(0, (int) tmp.size() - 1);
        for (int s = 0; s < swaps; ++s) {
            int i = dist(rng);
            int j = dist(rng);
            swap(tmp[i], tmp[j]);
        }
        tmp.resize(n);
        return tmp;
    }
};

void insertionSort(vi &a, int l, int r) {
    for (int i = l + 1; i < r; ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= l && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

void mergeRange(vi &a, int l, int m, int r, vi &buf) {
    int i = l, j = m, k = l;
    while (i < m && j < r) {
        if (a[i] <= a[j]) buf[k++] = a[i++];
        else buf[k++] = a[j++];
    }
    while (i < m) buf[k++] = a[i++];
    while (j < r) buf[k++] = a[j++];
    for (int p = l; p < r; ++p) a[p] = buf[p];
}

void mergeSortRec(vi &a, int l, int r, vi &buf) {
    if (r - l <= 1) return;
    int m = l + (r - l) / 2;
    mergeSortRec(a, l, m, buf);
    mergeSortRec(a, m, r, buf);
    mergeRange(a, l, m, r, buf);
}

void mergeSort(vi &a) {
    vi buf(a.size());
    mergeSortRec(a, 0, (int) a.size(), buf);
}

void hybridMergeRec(vi &a, int l, int r, vi &buf, int threshold) {
    int len = r - l;
    if (len <= 1) return;
    if (len <= threshold) {
        insertionSort(a, l, r);
        return;
    }
    int m = l + len / 2;
    hybridMergeRec(a, l, m, buf, threshold);
    hybridMergeRec(a, m, r, buf, threshold);
    mergeRange(a, l, m, r, buf);
}

void hybridMergeSort(vi &a, int threshold) {
    vi buf(a.size());
    hybridMergeRec(a, 0, (int) a.size(), buf, threshold);
}


struct Stats {
    double mean_ms;
    double median_ms;
    double min_ms;
    double max_ms;
    int runs;
};

struct SortTester {
    template<typename F>
    static Stats timeRuns(F runFunc, const vi &input, int repeats = 7) {
        vector<double> ms;
        ms.reserve(repeats);
        for (int t = 0; t < repeats; ++t) {
            vi a = input;
            auto start = chrono::high_resolution_clock::now();
            runFunc(a);
            auto elapsed = chrono::high_resolution_clock::now() - start;
            double msec = chrono::duration_cast<chrono::duration<double, milli> >(elapsed).count();
            ms.push_back(msec);
        }
        sort(ms.begin(), ms.end());
        double sum = 0;
        for (double v: ms) sum += v;
        Stats s;
        s.mean_ms = sum / ms.size();
        s.median_ms = ms[ms.size() / 2];
        s.min_ms = ms.front();
        s.max_ms = ms.back();
        s.runs = (int) ms.size();
        return s;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ArrayGenerator gen(12345, 100000, 0, 6000);

    vector<int> sizes = {500, 1500, 5000, 20000, 100000};

    vector<int> thresholds = {5, 10, 15, 20, 30, 50};

    cout << "type,n,threshold,mean_ms,median_ms,min_ms,max_ms,runs\n";

    for (int n: sizes) {
        vi arr_rand = gen.random_prefix(n);
        vi arr_rev = gen.reverse_prefix(n);
        vi arr_almost = gen.almost_sorted_prefix(n, max(1, n / 1000));

        {
            Stats st = SortTester::timeRuns([&](vi &a) { mergeSort(a); }, arr_rand, 7);
            cout << "random," << n << ",-," << st.mean_ms << "," << st.median_ms << "," << st.min_ms << "," << st.max_ms
                    << "," << st.runs << "\n";
        } {
            Stats st = SortTester::timeRuns([&](vi &a) { mergeSort(a); }, arr_rev, 7);
            cout << "reverse," << n << ",-," << st.mean_ms << "," << st.median_ms << "," << st.min_ms << "," << st.
                    max_ms << "," << st.runs << "\n";
        } {
            Stats st = SortTester::timeRuns([&](vi &a) { mergeSort(a); }, arr_almost, 7);
            cout << "almost," << n << ",-," << st.mean_ms << "," << st.median_ms << "," << st.min_ms << "," << st.max_ms
                    << "," << st.runs << "\n";
        }

        for (int thr: thresholds) {
            {
                Stats st = SortTester::timeRuns([&](vi &a) { hybridMergeSort(a, thr); }, arr_rand, 7);
                cout << "random," << n << "," << thr << "," << st.mean_ms << "," << st.median_ms << "," << st.min_ms <<
                        "," << st.max_ms << "," << st.runs << "\n";
            } {
                Stats st = SortTester::timeRuns([&](vi &a) { hybridMergeSort(a, thr); }, arr_rev, 7);
                cout << "reverse," << n << "," << thr << "," << st.mean_ms << "," << st.median_ms << "," << st.min_ms <<
                        "," << st.max_ms << "," << st.runs << "\n";
            } {
                Stats st = SortTester::timeRuns([&](vi &a) { hybridMergeSort(a, thr); }, arr_almost, 7);
                cout << "almost," << n << "," << thr << "," << st.mean_ms << "," << st.median_ms << "," << st.min_ms <<
                        "," << st.max_ms << "," << st.runs << "\n";
            }
        }
    }

    return 0;
}
