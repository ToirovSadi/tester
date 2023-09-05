





#include <bits/stdc++.h>
#include <mutex>

#include "config/config.cpp"

using namespace std;
using namespace std::chrono;

mutex mtx;
void copy_stream(stringstream& source, stringstream& dest);
int check_output(stringstream& fout_main, stringstream& target_fout);
void save_test(stringstream& input, stringstream& target_fout, stringstream& fout_main);



int cur_saved_tests = 0;



int check_output(stringstream& fout_main, stringstream& target_fout){
    if(fout_main.str() == target_fout.str()){
        return 0;
    }
    return 1;
}

string get_randstr(int str_size=5){
    srand(time(0));
    string s = "";
    while(str_size-- > 0) s += rand() % 26 + 'a';
    return s;
}

void save_test(stringstream& input, stringstream& target_fout, stringstream& fout_main){
    mtx.lock();
    const int saved_test = cur_saved_tests;
    mtx.unlock();
    if(saved_test >= config.total_saved_tests) return;
    cout << "Failed the Test! Saving the Test!...";
    int ok = 1;
    while(ok){
        string id = get_randstr();
        ok = system(("cd " + config.test_dir + " && mkdir " + id).c_str());
        if(ok == 0){
            // save the input
            std::ofstream in(config.test_dir + "/" + id + "/input.txt");
            in << input.rdbuf();

            // save the target output
            std::ofstream tout(config.test_dir + "/" + id + "/expected_output.txt");
            tout << target_fout.rdbuf();

            // save the main program output
            std::ofstream mout(config.test_dir + "/" + id + "/incorrect_output.txt");
            mout << fout_main.rdbuf();
        }
    }
    mtx.lock();
    cur_saved_tests ++;
    mtx.unlock();
    cout << "Done!\n";
}

void copy_stream(stringstream& source, stringstream& dest){
	dest << source.str();
}
namespace test_generator{
void test_generator(ostream& cout){
    srand(time(NULL));

    int n_test = 10;
    cout << n_test << "\n";
    for(int i = 1; i <= n_test; i ++){
        cout << rand() % 100 << " ";
    }
    cout << "\n";
}

}

namespace main_program{
void main_program(istream& cin, ostream& cout){
    int n;
    cin >> n;
    int res = 0;
    for(int i = 0; i < n; i ++){
        int x;
        cin >> x;
        res += x;
    }
    cout << res << "\n";
}

}

namespace target_program{
void target_program(istream& cin, ostream& cout){
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
}

}

int main(){
    auto run_test = [](){
        stringstream fin, fin1;
        test_generator::test_generator(fin1);
        copy_stream(fin1, fin);

        // to save the test if it failes
        stringstream temp1;
        copy_stream(fin, temp1);

        stringstream main_fout;
        // thread main_program = thread(main_program::main_program, ref(fin1), ref(main_fout));
        main_program::main_program(fin1, main_fout);
        
        stringstream target_fout;
        // thread target_program = thread(target_program::target_program, ref(fin), ref(target_fout));
        target_program::target_program(fin, target_fout);

        // main_program.join();
        // target_program.join();
        if(check_output(main_fout, target_fout) == 0){
            if(!config.save_only_failed) save_test(temp1, target_fout, main_fout);
            return 0;
        }
        // if not output is not satisfied save the test
        save_test(temp1, target_fout, main_fout);
        return 1;
    };


    unordered_map<string, int> statistics;
    statistics["WA"] = 0;
    statistics["TL"] = 0;
    statistics["AC"] = 0;
    statistics["total"] = 0;
    auto start = high_resolution_clock::now();
    auto last_start = start;

    int numThreads = 1;
    if(config.optimize){
        cout << "Optimizing Mode Is ON\n";
        numThreads = thread::hardware_concurrency();
        if(numThreads == 0){
            numThreads = 1;
        }else{
            numThreads --;
        }
        numThreads = max(1, int(numThreads * config.optimize_strength));

        cout << "Using " << numThreads << " threads\n\n";
    }
    int test_count = config.test_count;
    cout << "Running generator...\n";
    if(numThreads == 1){
        while(test_count > 0 || test_count == -1){
            int status = run_test();
            if(status == 0){
                statistics["AC"] ++;
            }else{
                statistics["WA"] ++;
            }

            if(test_count > 0) test_count --;
            statistics["total"] ++;
            
            auto duration = (high_resolution_clock::now() - last_start);
            if(duration_cast<seconds>(duration).count() > config.print_interval){
                cout << "[" << duration_cast<seconds>(high_resolution_clock::now() - start).count() << "s] ";
                cout << "By now " << statistics["AC"] << "/" << statistics["total"] << " tests passed\n";
                last_start = high_resolution_clock::now();
            }
        }
    }else{

        deque<thread> my_threads;
        try{
            while(test_count > 0 || test_count == -1){
                if((int)my_threads.size() < numThreads){
                    try{
                        my_threads.push_back(thread(run_test));
                        if(test_count > 0) test_count --;
                        statistics["total"] ++; 
                    }catch(exception e){
                        
                    }
                }else{
                    if(my_threads.front().joinable()){
                        my_threads.front().join();
                        my_threads.pop_front();
                    }
                    continue;
                }
                

                
                auto duration = (high_resolution_clock::now() - last_start);
                if(duration_cast<seconds>(duration).count() > config.print_interval){
                    cout << "[" << duration_cast<seconds>(high_resolution_clock::now() - start).count() << "s] ";
                    cout << "By now " << statistics["total"] << " tests are running\n";
                    last_start = high_resolution_clock::now();
                }
            }
        }catch(exception e){
            cout << "Stopping the generator...";
        }
        // wait for all threads to finish
        for(int i = 0; i < (int)my_threads.size(); i ++){
            if(my_threads[i].joinable()){
                my_threads[i].join();
            }
        }
    }


    cout << "Done!\n";
    cout << "Summary:\n";
    cout << "=======================================\n";
    cout << "total_test: " << statistics["total"] << "\n";
    if(numThreads == 1){
        cout << "AC: " << statistics["AC"] << "\n";
        cout << "WA: " << statistics["WA"] << "\n";
        cout << "TL: " << statistics["TL"] << "\n";
    }
    auto duration = (high_resolution_clock::now() - start);
    cout << "total time: " << duration_cast<seconds>(duration).count() << "s\n";
    cout << "=======================================\n";
}

