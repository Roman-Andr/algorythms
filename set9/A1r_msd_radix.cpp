#include <bits/stdc++.h>
using namespace std;

constexpr int ASCII_RANGE = 256;
constexpr int KEYS = ASCII_RANGE + 1;

int keyAt(const string &s, int d)
{
  if (d >= (int)s.size())
    return 0;
  return (unsigned char)s[d] + 1;
}

void msdRadixSort(vector<string> &a, vector<string> &aux, int lo, int hi, int d)
{
  if (lo >= hi)
    return;

  int count[KEYS] = {0};
  for (int i = lo; i <= hi; ++i)
    ++count[keyAt(a[i], d)];

  int start[KEYS + 1] = {0};
  for (int k = 0; k < KEYS; ++k)
    start[k + 1] = start[k] + count[k];

  int next[KEYS];
  for (int k = 0; k < KEYS; ++k)
    next[k] = start[k];

  for (int i = lo; i <= hi; ++i)
  {
    int k = keyAt(a[i], d);
    aux[lo + next[k]++] = move(a[i]);
  }
  for (int i = lo; i <= hi; ++i)
    a[i] = move(aux[i]);

  for (int k = 1; k < KEYS; ++k)
  {
    int bucketLo = lo + start[k];
    int bucketHi = lo + start[k + 1] - 1;
    if (bucketLo < bucketHi)
      msdRadixSort(a, aux, bucketLo, bucketHi, d + 1);
  }
}

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n))
    return 0;
  vector<string> a(n), aux(n);
  for (string &s : a)
    cin >> s;

  if (!a.empty())
    msdRadixSort(a, aux, 0, n - 1, 0);

  for (const string &s : a)
    cout << s << '\n';
  return 0;
}
