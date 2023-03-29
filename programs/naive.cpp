#include <bits/stdc++.h>

using namespace std;

int main(){
    int n;
    cin >> n;
    vector<int> a(n);
    for(auto &x: a) cin >> x;
    for(auto x: a) cout << x + 1 << " ";
    cout << "\n";
}