#include <bits/stdc++.h>
using namespace std;

constexpr int ASCII_RANGE = 256;
constexpr int KEYS = ASCII_RANGE + 1;
constexpr int ALPHABET_POWER = 74;

int stringQuickCharAt(const string &s, int d)
{
  if (d >= (int)s.size())
    return -1;
  return (unsigned char)s[d];
}

int radixKeyAt(const string &s, int d)
{
  if (d >= (int)s.size())
    return 0;
  return (unsigned char)s[d] + 1;
}

void stringQuickSort(vector<string> &a, int lo, int hi, int d)
{
  if (lo >= hi)
    return;

  int mid = lo + (hi - lo) / 2;
  swap(a[lo], a[mid]);

  int lt = lo, gt = hi;
  int pivot = stringQuickCharAt(a[lo], d);
  int i = lo + 1;

  while (i <= gt)
  {
    int cur = stringQuickCharAt(a[i], d);
    if (cur < pivot)
    {
      swap(a[lt++], a[i++]);
    }
    else if (cur > pivot)
    {
      swap(a[i], a[gt--]);
    }
    else
    {
      ++i;
    }
  }

  stringQuickSort(a, lo, lt - 1, d);
  if (pivot >= 0)
    stringQuickSort(a, lt, gt, d + 1);
  stringQuickSort(a, gt + 1, hi, d);
}

void msdRadixQuickSort(vector<string> &a, vector<string> &aux, int lo, int hi, int d)
{
  if (lo >= hi)
    return;
  if (hi - lo + 1 < ALPHABET_POWER)
  {
    stringQuickSort(a, lo, hi, d);
    return;
  }

  int count[KEYS] = {0};
  for (int i = lo; i <= hi; ++i)
    ++count[radixKeyAt(a[i], d)];

  int start[KEYS + 1] = {0};
  for (int k = 0; k < KEYS; ++k)
    start[k + 1] = start[k] + count[k];

  int next[KEYS];
  for (int k = 0; k < KEYS; ++k)
    next[k] = start[k];

  for (int i = lo; i <= hi; ++i)
  {
    int k = radixKeyAt(a[i], d);
    aux[lo + next[k]++] = move(a[i]);
  }
  for (int i = lo; i <= hi; ++i)
    a[i] = move(aux[i]);

  for (int k = 1; k < KEYS; ++k)
  {
    int bucketLo = lo + start[k];
    int bucketHi = lo + start[k + 1] - 1;
    if (bucketLo < bucketHi)
      msdRadixQuickSort(a, aux, bucketLo, bucketHi, d + 1);
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
    msdRadixQuickSort(a, aux, 0, n - 1, 0);

  for (const string &s : a)
    cout << s << '\n';
  return 0;
}
