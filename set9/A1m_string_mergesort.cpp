#include <bits/stdc++.h>
using namespace std;

int lcpFrom(const string &a, const string &b, int start)
{
  int i = max(0, start);
  while (i < (int)a.size() && i < (int)b.size() && a[i] == b[i])
    ++i;
  return i;
}

int compareFrom(const string &a, const string &b, int start)
{
  int i = max(0, start);
  while (i < (int)a.size() && i < (int)b.size())
  {
    unsigned char ca = (unsigned char)a[i];
    unsigned char cb = (unsigned char)b[i];
    if (ca < cb)
      return -1;
    if (ca > cb)
      return 1;
    ++i;
  }
  if (a.size() == b.size())
    return 0;
  return a.size() < b.size() ? -1 : 1;
}

void mergeWithLcp(vector<string> &a, vector<string> &aux, int lo, int mid, int hi)
{
  int i = lo, j = mid + 1, k = lo;

  bool hasLast = false;
  string last;
  int lcpLeft = 0, lcpRight = 0;

  auto refreshLcp = [&]()
  {
    if (!hasLast)
    {
      lcpLeft = lcpRight = 0;
      return;
    }
    if (i <= mid)
      lcpLeft = lcpFrom(last, a[i], 0);
    if (j <= hi)
      lcpRight = lcpFrom(last, a[j], 0);
  };

  while (i <= mid && j <= hi)
  {
    bool takeLeft;
    if (hasLast && lcpLeft > lcpRight)
    {
      takeLeft = true;
    }
    else if (hasLast && lcpLeft < lcpRight)
    {
      takeLeft = false;
    }
    else
    {
      int start = hasLast ? lcpLeft : 0;
      takeLeft = compareFrom(a[i], a[j], start) <= 0;
    }

    if (takeLeft)
      aux[k++] = move(a[i++]);
    else
      aux[k++] = move(a[j++]);

    last = aux[k - 1];
    hasLast = true;
    refreshLcp();
  }

  while (i <= mid)
    aux[k++] = move(a[i++]);
  while (j <= hi)
    aux[k++] = move(a[j++]);
  for (int p = lo; p <= hi; ++p)
    a[p] = move(aux[p]);
}

void stringMergeSort(vector<string> &a, vector<string> &aux, int lo, int hi)
{
  if (lo >= hi)
    return;
  int mid = lo + (hi - lo) / 2;
  stringMergeSort(a, aux, lo, mid);
  stringMergeSort(a, aux, mid + 1, hi);
  mergeWithLcp(a, aux, lo, mid, hi);
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
    stringMergeSort(a, aux, 0, n - 1);

  for (const string &s : a)
    cout << s << '\n';
  return 0;
}
