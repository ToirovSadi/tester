# configuration that you can change
check_solution = False

# save tests configuration
max_tests_to_save = 3
save_to_file = False  # If True, will not output test case it will be save in file in dir `tests_dir`
save_only_failed = True
input_file_name = 'input'
output_file_name = 'output'

program_name = 'main'
naive_program_name = 'naive'
test_generator_name = 'test_generator'
check_solution_name = 'check_solution'

tests_dir = 'tests/'
program_dir = 'programs/'
test_generator_file = test_generator_name + '.cpp'
check_solution_file = check_solution_name + '.cpp'

# options are ['python', 'g++', 'gcc']
program_compilers = {
    '.cpp': 'g++',
    '.c': 'gcc',
    '.py': 'python',
}
