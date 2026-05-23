#include <bits/stdc++.h>
using namespace std;

int charAt(const string &s, int d)
{
  if (d >= (int)s.size())
    return -1;
  return (unsigned char)s[d];
}

void stringQuickSort(vector<string> &a, int lo, int hi, int d)
{
  if (lo >= hi)
    return;

  int mid = lo + (hi - lo) / 2;
  swap(a[lo], a[mid]);

  int lt = lo, gt = hi;
  int pivot = charAt(a[lo], d);
  int i = lo + 1;

  while (i <= gt)
  {
    int cur = charAt(a[i], d);
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

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  int n;
  if (!(cin >> n))
    return 0;
  vector<string> a(n);
  for (string &s : a)
    cin >> s;

  if (!a.empty())
    stringQuickSort(a, 0, n - 1, 0);

  for (const string &s : a)
    cout << s << '\n';
  return 0;
}
