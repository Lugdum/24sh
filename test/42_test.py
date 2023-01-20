from argparse import ArgumentParser
from pathlib import Path
from os import listdir
from os.path import isfile, join
from difflib import unified_diff
from dataclasses import dataclass

import subprocess
import termcolor
import yaml

#OK_TAG = "[" + termcolor.colored('OK', 'green') + "]" # modified with chatgpt
#KO_TAG = "[" + termcolor.colored('KO', 'red') + "]"

OK_TAG = f"[{termcolor.colored('OK', 'green')}]"
KO_TAG = f"[{termcolor.colored('KO', 'red')}]"


@dataclass
class TestCase:
    file: str
    stdout: str
    stderr: bool
    exitcode: int


def diff(expected: str, actual: str) -> str:
    expected_lines = expected.splitlines(keepends=True)
    actual_lines = actual.splitlines(keepends=True)
    return ''.join(unified_diff(expected_lines, actual_lines, fromfile="expected", tofile="actual"))


def run_shell(*cmd_line) -> subprocess. CompletedProcess:
    return subprocess.run(cmd_line, capture_output=True, text=True)


def perform_checks(expected: subprocess.CompletedProcess, actual: subprocess.CompletedProcess):
    assert (not expected.stderr) or actual.stderr != "", \
    "Something was expected on stderr\n"

    assert expected.returncode == actual.returncode, \
    f"Exited with {actual.returncode} expected {expected.returncode}\n"

    assert expected.stdout == actual.stdout, \
    f"stdout differ\n{diff(expected.stdout, actual.stdout)}"


if __name__ == "__main__":
    parser = ArgumentParser("TestSuite")
    parser.add_argument ("--binary", required=True, type=Path)
    args = parser.parse_args()

    binary_path = args.binary.absolute()

    test_repo = "simple_test"
    test_files = [join(test_repo, f) for f in listdir(test_repo) if isfile (join(test_repo, f))]

    for testcase in test_files:
        file = testcase

        sh_proc = run_shell(binary_path, file)
        sh_ref = run_shell("bash", "--posix", file)

        try:
            perform_checks (sh_ref, sh_proc)
        except AssertionError as err:
            print(f"{KO_TAG} {file}\n{err}")
            print(sh_proc)
            print(sh_ref)
        else:
            print(f"{OK_TAG} {file}\n")
            pass