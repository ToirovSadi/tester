#include <bits/stdc++.h> // #remove

using namespace std; // #remove

// #remove please don't remove labels
int main(){ // #begin_main
    int n;
    cin >> n;
    int res = 0;
    for(int i = 0; i < n; i ++){
        int x;
        cin >> x;
        res += x;
    }
    cout << res << "\n";
} // #end_main