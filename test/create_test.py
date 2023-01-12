from os import listdir
from os.path import isfile, join
from dataclasses import dataclass, asdict

import subprocess
import yaml


@dataclass
class TestCase:
    file: str
    stdout: str
    stderr: bool
    exitcode: int


def run_shell(file: str) -> subprocess.CompletedProcess:
    return subprocess.run(["bash", "--posix", file], capture_output=True, text=True)

def create_testcase_from_file(file: str) -> TestCase:
    process = run_shell(file)
    return TestCase(file, process.stdout, process.stderr != "", process.returncode)

if __name__ == "__main__":
    test_repo = "simple_test"
    test_files = [join(test_repo, f) for f in listdir(test_repo) if isfile(join(test_repo, f))]
    test_cases = [create_testcase_from_file(f) for f in test_files]
    test_dicts = [asdict (case) for case in test_cases]
    with open('data.yml', 'w') as outfile:
        yaml.dump(test_dicts, outfile)