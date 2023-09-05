#include <bits/stdc++.h>
#include "config/config.cpp"

using namespace std;
using namespace std::chrono;

struct ProgramStr{
    string main;
    string global_code;
    string function_begin;
    string function_end;
};
unordered_map<string, ProgramStr> generated_code;

void setup_config(){
    cout << "Setting up config... ";
    if(!check_config()){
        cout << "Error in config file!\n";
        exit(1);
    }
    cout << "Done!\n";
}

void add_line(string program_name, string code, bool in_main=true){
    if(in_main){
        generated_code[program_name].main += code;
    }else{
        generated_code[program_name].global_code += code;
    }
}

void copy_stream(stringstream& source, stringstream& dest){
    dest << source.str();
}

void copy_code(string program_name, string from){
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

void add_target(string program_name){
    generated_code[program_name].function_begin += "void target_program(istream& cin, ostream& cout){\n";

    copy_code(program_name, config.target_program);
    
    generated_code[program_name].function_end += "}\n\n";
}

void add_main(string program_name){
    generated_code[program_name].function_begin += "void main_program(istream& cin, ostream& cout){\n";

    copy_code(program_name, config.main_program);

    generated_code[program_name].function_end += "}\n\n";
}

void add_test_generator(string program_name){
    generated_code[program_name].function_begin += "void test_generator(ostream& cout){\n";

    copy_code(program_name, config.test_generator);

    generated_code[program_name].function_end += "}\n\n";
}

void generate_code(){
    cout << "Generating code... ";
    generated_code.clear();

    string program_name = config.generated_code;
    generated_code[program_name].function_begin += "int main(){\n";

    copy_code(program_name, config.test_template);

    generated_code[program_name].function_end += "}\n\n";

    add_main(config.main_program);
    add_target(config.target_program);
    add_test_generator(config.test_generator);

    ofstream codeFile(config.generated_code);
    codeFile << generated_code[config.test_generator].global_code;
    codeFile << generated_code[config.main_program].global_code;
    codeFile << generated_code[config.target_program].global_code;
    codeFile << generated_code[config.generated_code].global_code;

    // add test_generator main function
    codeFile << "namespace test_generator{\n";
    codeFile << generated_code[config.test_generator].function_begin;
    codeFile << generated_code[config.test_generator].main;
    codeFile << generated_code[config.test_generator].function_end;
    codeFile << "}\n\n";

    // add main_program main function
    codeFile << "namespace main_program{\n";
    codeFile << generated_code[config.main_program].function_begin;
    codeFile << generated_code[config.main_program].main;
    codeFile << generated_code[config.main_program].function_end;
    codeFile << "}\n\n";

    // add target_program main function
    codeFile << "namespace target_program{\n";
    codeFile << generated_code[config.target_program].function_begin;
    codeFile << generated_code[config.target_program].main;
    codeFile << generated_code[config.target_program].function_end;
    codeFile << "}\n\n";

    // add generated_code main function
    codeFile << generated_code[config.generated_code].function_begin;
    codeFile << generated_code[config.generated_code].main;
    codeFile << generated_code[config.generated_code].function_end;

    codeFile.close();
    cout << "Done!\n";
}

void run_generator(){
    string command = "./generated_code 2> /dev/null";
    system(command.c_str());
}

string remove_extension(string filename){
    size_t dotPos = filename.find_last_of(".");
    if(dotPos != string::npos){
        filename = filename.substr(0, dotPos);
    }
    return filename;
}

void check_program(string program_name, bool remove_executable=true, bool add_pthread=false){
    int exit_code = 0;
    cout << "Checking " << program_name << "... ";
    string program = remove_extension(program_name);
    string command = "";
    if(add_pthread)
        command = "g++ " + program_name + " -o " + program + " -pthread > /dev/null 2> /dev/null";
    else
        command = "g++ " + program_name + " -o " + program + " > /dev/null 2> /dev/null";

    exit_code = system(command.c_str());
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
    check_program(config.main_program);
    check_program(config.target_program);
    check_program(config.test_generator);
    check_program(config.test_template, true, true);
    check_program(config.generated_code, false, true);
    cout << "All programs compiled successfully!\n";
}

int main(){
    setup_config();
    generate_code();

    if(config.check_all_programs){
        check_programs();
    }

    run_generator();
} 