#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

void f(){
    cout << "hey\n";
    sleep(3);
    return;
}

int main(){
    thread t;
    cout << t.joinable() << "\n";
    t = thread(f);
    cout << t.joinable() << "\n";
    sleep(5);
    cout << t.joinable() << "\n";

    t.join();
}