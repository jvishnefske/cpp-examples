#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

using namespace std;

typedef long long ll;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;

#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define REP(i, n) for (int i = 0; i < (n); i++)
#define TR(e, x) for (auto& e : x)
#define DEBUG(C) cerr << #C << " = " << C << endl;
#define DUMP(x)
class Solution {
public:
  int maxProfit(vector<int>& prices) {
    int n = prices.size();
    if (n == 0) {
      return 0;
    }
    int min_price = prices[0];
    int max_profit = 0;
    for (int i = 1; i < n; i++) {
      if (prices[i] < min_price) {
        min_price = prices[i];
      } else {
        max_profit = max(max_profit, prices[i] - min_price);
      }
    }
    return max_profit;
  }
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(0);
  Solution s;
  vector<int> prices = {7, 1, 5, 3, 6, 4};
  cout << s.maxProfit(prices) << endl;
  return 0;
}
