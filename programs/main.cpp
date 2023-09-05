#include <bits/stdc++.h> // #remove

using namespace std; // #remove

// #remove please don't remove labels
int main(){ // #begin_main
    srand(time(NULL));
    for(int i = 1; i <= 1e7; i ++);
    int n;
    cin >> n;
    int res = 0;
    for(int i = 0; i < n; i ++){
        int x;
        cin >> x;
        res += x;
    }
    if(rand() % 4 == 0){
        res --;
    }
    cout << res << "\n";
} // #end_main
