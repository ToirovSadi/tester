#include <string>
#include <fstream>

struct _config{
    // files to merge
    const std::string target_program = "programs/main.cpp";
    const std::string main_program = "programs/naive.cpp";
    const std::string test_generator = "test_generator.cpp";
    const std::string test_template = "test_template.cpp";

    // generated_code is the file where the generated code will be saved
    const std::string generated_code = "generated_code.cpp";

    // time_limit is the maximum time that the program will run (in seconds)
    const int time_limit = 2;

    // memory_limit is the maximum memory that the program can occupy (in mb)
    const int memory_limit = 256;

    // number of tests to tun, test_count=-1 means that it will run forever
    const int test_count = 1000;

    // optimizer checking by creating new threads
    const bool optimize = true;
    const float optimize_strength = 1; // use 80% of the threads

    // test_dir is the directory where all the tests will be saved
    const std::string test_dir = "tests";

    // save_only_failed=true means it will save only failed tests
    const bool save_only_failed = true;

    // total_saved_tests is the number of tests that will be saved
    const int total_saved_tests = 10;

    // print_interval is the interval at which the test results will be printed (in seconds)
    const int print_interval = 3;

    // check all programs before runnning generator (recommended)
    const bool check_all_programs = true;  
} config;

bool _file_exists(std::string filename){
    std::ifstream f(filename);
    return f.good();
}

bool check_config(){
    bool all_ok = true;
    bool ok = true;
    
    // check if all the files exist
    ok &= _file_exists(config.target_program);
    ok &= _file_exists(config.main_program);
    ok &= _file_exists(config.test_generator);
    ok &= _file_exists(config.test_template);
    ok &= _file_exists(config.generated_code);
    if(!ok) printf("error:config.cpp:one or more files are missing\n");
    all_ok &= ok; ok = true;

    // check if the time limit is positive and not above 10 seconds
    ok &= config.time_limit > 0;
    ok &= config.time_limit <= 10;
    if(!ok) printf("erorr:config.cpp:time limit must be between 1 and 10 seconds\n");
    all_ok &= ok; ok = true;

    // check if the memory limit is positive and not above 2048 mb
    ok &= config.memory_limit > 0;
    ok &= config.memory_limit <= 2048;
    if(!ok) printf("erorr:config.cpp:memory limit must be between 1 and 2048 mb\n");
    all_ok &= ok; ok = true;

    // check if the test count is positive or test_count=-1
    ok &= config.test_count == -1 || config.test_count > 0;
    if(!ok) printf("erorr:config.cpp:test count must be positive or -1\n");
    all_ok &= ok; ok = true;

    // check if the optimize strength is between 0 and 1
    ok &= (config.optimize_strength >= 0 && config.optimize_strength <= 1);
    if(!ok) printf("erorr:config.cpp:optimize strength must be between 0 and 1\n");
    all_ok &= ok; ok = true;

    // check if the test directory exists
    ok &= _file_exists(config.test_dir);
    if(!ok) printf("erorr:config.cpp:test directory does not exist\n");
    all_ok &= ok; ok = true;

    // check if the total saved tests is positive
    ok &= config.total_saved_tests > 0;
    if(!ok) printf("erorr:config.cpp:total saved tests must be positive\n");
    all_ok &= ok; ok = true;

    // check if the print interval is positive
    ok &= config.print_interval > 0;
    if(!ok) printf("erorr:config.cpp:print interval must be positive\n");
    all_ok &= ok; ok = true;

    return all_ok;
}