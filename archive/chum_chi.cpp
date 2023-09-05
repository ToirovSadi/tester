#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <mutex>

using namespace std;
using namespace std::chrono;

mutex mtx;
unordered_map<string, string> config;
unordered_map<string, int> statistics;

struct ProgramStr{
    string main;
    string global_code;
    string function_begin;
    string function_end;
};
unordered_map<string, ProgramStr> generated_code;

void setup_config(){
    cout << "Setting up config... ";
    ifstream configFile("config.txt");
    string line;
    while (getline(configFile, line)) {
        if (line.empty()) continue;

        // if this line is comment skip it
        if (line[0] == '/') continue;

        size_t equalsPos = line.find('=');
        if (equalsPos != string::npos) {
            string key = line.substr(0, equalsPos);
            string value = line.substr(equalsPos + 1);
            config[key] = value;
        }
    }
    cout << "Done!\n";
}

void add_line(string& program_name, string code, bool in_main=true){
    if(in_main){
        generated_code[program_name].main += code;
    }else{
        generated_code[program_name].global_code += code;
    }
}

void copy_stream(stringstream& source, stringstream& dest){
    dest << source.str();
}

void copy_code(string& program_name, string from){
    string line;
    ifstream targetFile(from);
    bool in_main = false;
    while(getline(targetFile, line)){
        if(line.find("#remove") != string::npos)
            continue;
        
        if(line.find("#begin_main") != string::npos){
            in_main = true;
            continue;
        }

        if(line.find("#end_main") != string::npos){
            in_main = false;
            continue;
        }

        add_line(program_name, line + "\n", in_main);
    }
    targetFile.close();
}

void add_target(string& program_name){
    generated_code[program_name].function_begin += "void target_program(istream& cin, ostream& cout){\n";

    copy_code(program_name, config["target_program"]);
    
    generated_code[program_name].function_end += "}\n\n";
}

void add_main(string& program_name){
    generated_code[program_name].function_begin += "void main_program(istream& cin, ostream& cout){\n";

    copy_code(program_name, config["main_program"]);

    generated_code[program_name].function_end += "}\n\n";
}

void add_test_generator(string& program_name){
    generated_code[program_name].function_begin += "void test_generator(ostream& cout){\n";

    copy_code(program_name, config["test_generator"]);

    generated_code[program_name].function_end += "}\n\n";
}

void generate_code(){
    cout << "Generating code... ";
    generated_code.clear();

    string program_name = "generated_code.cpp";
    generated_code[program_name].function_begin += "int main(){\n";

    copy_code(program_name, config["test_template"]);

    generated_code[program_name].function_end += "}\n\n";

    add_main(config["main_program"]);
    add_target(config["target_program"]);
    add_test_generator(config["test_generator"]);

    ofstream codeFile("generated_code.cpp");

    codeFile << generated_code[config["generated_code"]].global_code;
    codeFile << generated_code[config["test_generator"]].global_code;
    codeFile << generated_code[config["main_program"]].global_code;
    codeFile << generated_code[config["target_program"]].global_code;

    // add test_generator main function
    codeFile << "namespace test_generator{\n";
    codeFile << generated_code[config["test_generator"]].function_begin;
    codeFile << generated_code[config["test_generator"]].main;
    codeFile << generated_code[config["test_generator"]].function_end;
    codeFile << "}\n\n";

    // add main_program main function
    codeFile << "namespace main_program{\n";
    codeFile << generated_code[config["main_program"]].function_begin;
    codeFile << generated_code[config["main_program"]].main;
    codeFile << generated_code[config["main_program"]].function_end;
    codeFile << "}\n\n";

    // add target_program main function
    codeFile << "namespace target_program{\n";
    codeFile << generated_code[config["target_program"]].function_begin;
    codeFile << generated_code[config["target_program"]].main;
    codeFile << generated_code[config["target_program"]].function_end;
    codeFile << "}\n\n";

    // add generated_code main function
    codeFile << generated_code[config["generated_code"]].function_begin;
    codeFile << generated_code[config["generated_code"]].main;
    codeFile << generated_code[config["generated_code"]].function_end;

    codeFile.close();
    cout << "Done!\n";
}

int run_program(string program_name){
    auto start = high_resolution_clock::now();
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        int exit_code = system(program_name.c_str());
        exit(exit_code);
    } else if (pid > 0) {
        // Parent process
        int status, result;
        int time_limit = 0;
        try{
            time_limit = stoi(config["time_limit"]);
        }catch(exception e){
            cout << "Error: time_limit in config.txt should be int, error while casting to int\n";
            time_limit = 5;
        }
        // cout << "waiting for " << pid << endl;
        while(true){
            result = waitpid(pid, &status, WNOHANG);
            if(result != 0){
                break;
            }
            sleep(0.02);
            auto duration = (high_resolution_clock::now() - start);
            if(duration_cast<seconds>(duration).count() > time_limit){
                break;
            }
        }
        // cout << "result: " << result << endl;
        if(result != 0){
            mtx.lock();
            statistics["TL"] += 1;
            mtx.unlock();
            kill(pid, SIGTERM);
            return 1;
        }
        
        mtx.lock();
        if(status == 0){
            statistics["AC"] += 1;
        }else{
            statistics["WA"] += 1;
        }
        mtx.unlock();
    } else {
        mtx.lock();
        statistics["WA"] += 1;
        mtx.unlock();
        return 1;
    }
    return 0;
}

void run_generator(){
    statistics["WA"] = 0;
    statistics["TL"] = 0;
    statistics["AC"] = 0;
    statistics["total"] = 0;
    statistics["cur_thread"] = 0;
    cout << "Running generator...\n";
    int test_count = stoi(config["test_count"]);
    int test_number = 1;
    float print_interval = stof(config["print_interval"]);
    auto start = high_resolution_clock::now();
    
    int numThreads = 1;
    if(config["optimize"] == "true"){
        numThreads = thread::hardware_concurrency();
        if(numThreads == 0){
            numThreads = 1;
        }else{
            numThreads --;
        }
    }

    deque<thread> my_threads;
    // system("g++ generated_code.cpp -o generated_code");
    try{
        while(test_count > 0 || test_count == -1){
            if(numThreads == 1){
                run_program("./generated_code");
            }else{
                if((int)my_threads.size() < numThreads){
                    try{
                        my_threads.push_back(thread(run_program, "./generated_code"));
                    }catch(exception e){
                        cout << "Error: " << e.what() << "\n";
                    }
                }else{
                    if(!my_threads.empty()){
                        if(my_threads.front().joinable()){
                            my_threads.front().join();
                            my_threads.pop_front();
                        }
                    }
                    continue;
                }
            }
            
            auto duration = (high_resolution_clock::now() - start);
            if(duration_cast<seconds>(duration).count() > print_interval){
                cout << "By now " << statistics["AC"] << "/" << statistics["total"] << " tests passed\n";
                start = high_resolution_clock::now();
            }

            if(test_count > 0) test_count --;
            statistics["total"] ++;
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
    // sleep(5);
    cout << "Done!\n";

    cout << "Summary:\n";
    cout << "=======================================\n";
    cout << "total_test: " << statistics["total"] << "\n";
    cout << "AC: " << statistics["AC"] << "\n";
    cout << "WA: " << statistics["WA"] << "\n";
    cout << "TL: " << statistics["TL"] << "\n";
    cout << "=======================================\n";
}

string remove_extension(string filename){
    size_t dotPos = filename.find_last_of(".");
    if(dotPos != string::npos){
        filename = filename.substr(0, dotPos);
    }
    return filename;
}

void check_program(string program_name, bool remove_executable=true){
    int exit_code = 0;
    cout << "Checking " << program_name << "... ";
    string program = remove_extension(program_name);
    exit_code = system(("g++ " + program_name + " -o " + program + " 2> /dev/null").c_str());
    if(exit_code != 0){
        cout << "Error while compiling " << program_name << "!\n";
        exit(0);
    }
    cout << "Done!\n";
    
    if(remove_executable){
        system(("rm " + program + " -f").c_str());
    }
}

void check_programs(){
    check_program(config["main_program"]);
    check_program(config["target_program"]);
    check_program(config["test_generator"]);
    check_program(config["test_template"]);
    check_program(config["generated_code"], false);
    cout << "All programs compiled successfully!\n";
}

int main(){
    setup_config();
    generate_code();

    if(config["check_all_programs"] == "true"){
        check_programs();
    }

    run_generator();

    // system("rm ./generated_code -f");
}