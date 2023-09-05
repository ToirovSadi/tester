#include <bits/stdc++.h> // #remove

using namespace std; // #remove

int main(){ // #begin_main
    srand(time(NULL));

    int n_test = 10;
    cout << n_test << "\n";
    for(int i = 1; i <= n_test; i ++){
        cout << rand() % 100 << " ";
    }
    cout << "\n";
} // #end_main