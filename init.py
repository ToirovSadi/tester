import subprocess
import os

import config

junk_files = []

program = None
naive_program = None
test_generator = None
check_solution = None


def check():
    global program
    global naive_program
    global test_generator
    global check_solution

    program_file_exist = False
    naive_file_exist = False
    for file in os.listdir(config.program_dir):
        if file.count('.') == 0:
            continue
        split_index = file.rfind('.')
        if file.startswith(config.program_name):
            program_file_exist = True
            program = (config.program_dir + file[:split_index], file[split_index:])
        if file.startswith(config.naive_program_name):
            naive_file_exist = True
            naive_program = (config.program_dir + file[:split_index], file[split_index:])

    if os.path.isfile(config.test_generator_file):
        split_index = config.test_generator_file.rfind('.')
        test_generator = (config.test_generator_file[:split_index], config.test_generator_file[split_index:])
    else:
        print(f'Error: can not find file {config.test_generator_name}')
        exit(1)

    if config.check_solution:
        if os.path.isfile(config.check_solution_file):
            split_index = config.check_solution_file.rfind('.')
            check_solution = (config.check_solution_file[:split_index], config.check_solution_file[split_index:])
        else:
            print(f'Error: can not find file {config.check_solution_file}')
            exit(1)

    if not program_file_exist:
        print(f'Error: can not find file {config.program_dir + config.program_name}')
        exit(1)
    if not naive_file_exist:
        print(f'Error: can not find file {config.program_dir + config.naive_program_name}')
        exit(1)


def build(file):
    result = None
    compiler = config.program_compilers.get(file[1], None)
    if compiler == 'g++' or compiler == 'gcc':
        result = subprocess.run([compiler, file[0] + file[1], "-o", file[0]], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        junk_files.append(file[0])
    elif compiler == 'python':
        # no need to build a program for python
        pass
    else:
        print(f'Error: can not find proper compiler, set of available compilers are {list(config.program_compilers.values())}')
        exit(1)

    if result is not None and result.returncode != 0:
        print("Compilation failed with the following error message:\n", result.stderr.decode())
        exit(1)


def init():
    check()
    build(program)
    build(naive_program)
    build(test_generator)
    if config.check_solution:
        build(check_solution)


def get_program():
    return program


def get_naive_program():
    return naive_program


def get_test_generator():
    return test_generator


def get_check_solution():
    return check_solution


def erase():
    for file in junk_files:
        os.remove(file)
