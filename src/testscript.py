import pathlib
import re
import subprocess

DATA = pathlib.Path('test')

def parser_output(spl_file):
    with open(spl_file, "r") as input:
        out = subprocess.check_output(['bin/splc'],stdin=input)

    return out.decode().strip()

if __name__=="__main__":
    data = DATA
    recovered, total = 0, 0
    # print(data.glob('test_1*.spl'))
    for spl_file in data.glob('test_1*.spl'):
        print("checking:",spl_file)
        program_out = parser_output(spl_file)
        with open(spl_file.with_suffix('.out'),'r') as output:
            sample=output.read()
            if sample.strip()!=program_out.strip():
                print('Fail at {}\n=========================\n{}\n==========================\n{}'.format(spl_file,sample,program_out))
            else:
                print('pass test {}'.format(spl_file))
            print('-'*70)

    # print("OK")
        # print("===============================")
        # print(out)
        