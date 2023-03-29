import utils
import config
import init
naive_program = None
default_n_tests = None


def set_up(program=None):
    global naive_program
    naive_program = program


def generate_from_other_program():
    test_generator = init.get_test_generator()
    run_program = utils.run(test_generator)
    output_data, _ = run_program.communicate()
    return output_data.decode()


# without output
def generate_single_test():
    if config.test_generator_file != config.test_generator_name + '.py':
        return generate_from_other_program()

    # generate test and convert it to string and return it
    test = '2\n1 2\n'
    return test


def generate_test():
    if naive_program is None:
        print('Error: please set up generator first!')
        exit(1)

    input_test = generate_single_test()
    run_program = utils.run(naive_program)
    input_data = input_test.encode()

    output_data, _ = run_program.communicate(input=input_data)

    return {'input': input_test, 'output': output_data.decode()}
