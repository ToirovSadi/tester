import subprocess

import config
import init


def clean_output(s):
    p = s.split('\n')
    for i in range(len(p)):
        while len(p[i]) > 0 and p[i][-1] == ' ':
            p[i] = p[i][:-1]
        while len(p[i]) > 0 and p[i][-1] == '\n':
            p[i] = p[i][:-1]
    return '\n'.join([i for i in p if len(i) > 0])


# a - expected, b - output
def equals(a, b):
    if config.check_solution:
        return apply_check_solution(b)

    return clean_output(a) == clean_output(b)


def apply_check_solution(b):
    check_solution = init.get_check_solution()
    run_program = run(check_solution)
    run_program.communicate(input=b.encode())
    return run_program.returncode == 0


def run(file):
    compiler = config.program_compilers[file[1]]

    if compiler == 'g++' or compiler == 'gcc':
        return subprocess.Popen(['./' + file[0]], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    if compiler == 'python':
        return subprocess.Popen([compiler, file[0] + file[1]], stdin=subprocess.PIPE, stdout=subprocess.PIPE)


def show_failed_test(test, show_input=True, show_expected=True, show_output=True):
    print('-Test-Failed!', '-'*35, sep='')
    if show_input:
        print('-'*15, 'Input', '-'*28, sep='')
        print(test['input'])

    if show_expected:
        print('-'*15, 'Expected', '-'*25, sep='')
        print(test['expected'])

    if show_output:
        print('-'*15, 'Output', '-'*28, sep='')
        print(test['output'])

    print('-End-of-Test', '-'*35)
    exit(1)
