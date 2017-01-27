from collections import defaultdict, namedtuple
import itertools
import os
import tempfile
import time
import subprocess
import sys

import numpy as np
import pandas as pd


RUNS_PER_FILE = 2
CODE_DIR = 'code'

ExecutionTime = namedtuple('ExecutionTime', ['exercise', 'extention', 'time'])


def call_code(*args, **kwargs):
    kwargs['cwd'] = CODE_DIR
    kwargs['stdin'] = subprocess.PIPE
    kwargs['stdout'] = subprocess.PIPE
    p = subprocess.Popen(*args, **kwargs)
    p.communicate()


def print_inline(*args, **kwargs):
    kwargs['end'] = ''
    print(*args, **kwargs)
    sys.stdout.flush()


def python_runner(exercise):
    """Generator that yields execution times."""
    while True:
        start = time.time()
        call_code(['python', f'{exercise}.py'])
        end = time.time()
        yield end - start


def cpp_runner(exercise):
    """Generator that yields execution times."""
    tempfile = '/tmp/cpp_executable'
    cmd = [
        'g++',
        '-std=c++11',
        '-Ilibs',
        '-DTHRUST_DEVICE_SYSTEM=THRUST_HOST_SYSTEM_OMP',
        '-fopenmp',
        f'-o{tempfile}',
        f'{exercise}.cpp',
    ]
    call_code(cmd)
    while True:
        start = time.time()
        call_code([tempfile])
        end = time.time()
        yield end - start


RUNNERS = {
    '.py': python_runner,
    '.cpp': cpp_runner
}


def collect_exercises():
    exercises = set()
    for filename in os.listdir(CODE_DIR):
        exercise, extention = os.path.splitext(filename)
        if extention in RUNNERS:
            exercises.add(exercise)
    return sorted(exercises)


def profile_pair(exercise, runs_per_file):
    runs = []
    print(f'Running {exercise}')
    for extention, runner in RUNNERS.items():
        print_inline(f'{extention}: ')
        ready = runner(exercise)
        for execution_time in itertools.islice(ready, runs_per_file):
            result = ExecutionTime(exercise=exercise, extention=extention,
                                   time=execution_time)
            runs.append(result)
            print_inline('.')
        print()
    return runs


def profile_all(runs_per_file):
    exercises = collect_exercises()
    return [result for bn in exercises
            for result in profile_pair(bn, runs_per_file)]


def analyze(profiles):
    return (
        pd
        .DataFrame(profiles)
        .groupby(by=['exercise', 'extention'])
        .mean()
    )


def main():
    profiles = profile_all(RUNS_PER_FILE)
    print(analyze(profiles))



if __name__ == '__main__':
    main()
