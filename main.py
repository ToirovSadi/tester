import datetime
import init
import test_generator
import utils
import config

cnt = 0
saved_tests = 0
test_failed = False

init.init()
program = init.get_program()
naive_program = init.get_naive_program()
test_generator.set_up(program=naive_program)
next_update = datetime.datetime.now() + datetime.timedelta(seconds=5)
print('Everything set up correctly, generating tests!')
try:
    while True:
        test_failed = False
        test = test_generator.generate_test()
        run_program = utils.run(program)
        input_data = test['input'].encode()

        output_program, _ = run_program.communicate(input=input_data)
        output_program = output_program.decode()
        if not utils.equals(test['output'], output_program):
            test_failed = True
            if not config.save_to_file:
                utils.show_failed_test(test={
                    'input': test['input'],
                    'expected': test['output'],
                    'output': output_program
                })

        if config.save_to_file and saved_tests < config.max_tests_to_save:
            if not config.save_only_failed or test_failed:
                saved_tests += 1
                fin = open(f'{config.tests_dir}{config.input_file_name}{str(saved_tests)}.txt', 'w')
                fin.write(test['input'])
                fin.close()

                fout = open(f'{config.tests_dir}{config.output_file_name}{str(saved_tests)}.txt', 'w')
                fout.write(test['output'])
                fout.close()
                print('test saved in', config.tests_dir)

        cnt += 1
        if datetime.datetime.now() >= next_update:
            print('By now', cnt, 'tests have passed!')
            next_update = datetime.datetime.now() + \
                          datetime.timedelta(seconds=5)

except KeyboardInterrupt:  # Ctrl + C
    init.erase()
