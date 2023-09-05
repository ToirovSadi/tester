#include<bits/stdc++.h>
#include<sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

using namespace std;
using namespace std::chrono;

int main(){ 
    // Define a structure for resource limits
    struct rlimit limit;
    
    // Set CPU time limit to 5 seconds
    limit.rlim_cur = 5; // Soft limit
    limit.rlim_max = 5; // Hard limit
    if(setrlimit(RLIMIT_CPU, &limit) != 0){
        // std::cerr << "Failed to set CPU time limit." << std::endl;
        return 1;
    }

    // Set memory limit to 1 GB (1,000,000 kilobytes)
    limit.rlim_cur = 1000000; // Soft limit
    limit.rlim_max = 1000000; // Hard limita
    if (setrlimit(RLIMIT_AS, &limit) != 0) {
        // std::cerr << "Failed to set resource limits." << std::endl;
        return 1;
    }

    auto start = high_resolution_clock::now();
    while(true){
        auto duration = (high_resolution_clock::now() - start);
        cout << duration_cast<seconds>(duration).count() << "\n";
    }
}