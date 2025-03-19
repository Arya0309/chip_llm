import os
import subprocess
from colorama import init, Fore
import argparse
import pandas as pd
import json

init(autoreset=True)


class GenCodeChecker:
    def __init__(self):
        self.table = pd.DataFrame(
            data=[[0, 0, 0, 0, 0]],
            columns=[
                "complete",
                "uncompilable",
                "execution_fail",
                "unit_test_fail",
                "timeout",
            ],
            dtype=int,
        )
        self.recorder = []

    def _check(self, dir, type: str = "cpp"):
        # 檢查目錄是否存在
        if not os.path.isdir(dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        if not self._check_compilable(dir, type):
            self.table[self.status] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": False,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.compile_result.stdout,
                }
            )
            return

        if not self._check_unit_test(dir):
            self.table[self.status] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": True,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.unit_test_result.stdout,
                }
            )
            return

        self.table[self.status] += 1
        self.recorder.append(
            {
                "dir": dir,
                "compilable": True,
                "unit_test_pass": True,
                "status": self.status,
                "error_msg": self.unit_test_result.stdout,
            }
        )
        return

    def _check_compilable(self, dir, type: str = "cpp"):
        files = set(os.listdir(dir))
        if type == "cpp":
            gcc_files = [f for f in files if f.endswith(".c") or f.endswith(".cpp")]
            result = subprocess.run(
                [
                    "g++",
                    "-o",
                    "main",
                    *gcc_files,
                    "-L/usr/local/systemc-2.3.3/lib-linux64",
                    "-I/usr/local/systemc-2.3.3/include",
                    "-lsystemc-2.3.3",
                    "-Wl,-rpath=/usr/local/systemc-2.3.3/lib-linux64",
                ],
                cwd=dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
            )
        elif type == "make":
            result = subprocess.run(
                ["make"], cwd=dir, stdout=subprocess.PIPE, stderr=subprocess.PIPE
            )
        else:
            print(Fore.RED + "Error: Invalid type")
            return False

        self.compile_result = result

        if result.returncode != 0:
            print(Fore.RED + "Compilation failed.")
            print(result.stdout.decode("utf-8", "ignore"))
            self.status = "uncompilable"
            return False
        else:
            print(Fore.GREEN + "Compilation successful!")
            return True

    def _check_unit_test(self, dir):
        try:
            result = subprocess.run(
                ["./main"],
                cwd=dir,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=5,
            )
            self.unit_test_result = result
        except subprocess.TimeoutExpired as e:
            result = e

        # 移除main檔案
        os.remove(os.path.join(dir, "main"))

        if isinstance(result, subprocess.TimeoutExpired):
            print(Fore.RED + "Timeout.")
            print(result.stdout.decode("utf-8", "ignore"))
            self.status = "timeout"
            return False
        elif "Error" in result.stdout.decode("utf-8", "ignore"):
            print(Fore.RED + "Excution failed.")
            print(result.stdout.decode("utf-8", "ignore"))
            self.status = "execution_fail"
            return False
        elif "fail" in result.stdout.decode("utf-8", "ignore"):
            print(Fore.RED + "Unit test failed.")
            print(result.stdout.decode("utf-8", "ignore"))
            self.status = "unit_test_fail"
            return False
        else:
            print(Fore.GREEN + "Unit test successful!")
            self.status = "complete"
            return True

    def _write_record(self):
        os.makedirs(".log", exist_ok=True)
        log_path = os.path.join(self.log_dir, ".log")

        file_path = os.path.join(log_path, "compile_check_result.csv")
        if os.path.exists(file_path):
            df = pd.read_csv(file_path, index_col=0)
            self.table = pd.concat([df, self.table], axis=0)
            self.table.reset_index(drop=True, inplace=True)
        self.table.to_csv(file_path)
        print(self.table)

        index = len(self.table) - 1
        for dict in self.recorder:
            if isinstance(dict["error_msg"], bytes):
                dict["error_msg"] = dict["error_msg"].decode("utf-8", "ignore")
        with open(
            os.path.join(log_path, f"compile_check_result_{index}.json"),
            "w",
            encoding="utf-8",
        ) as f:
            json.dump(self.recorder, f, indent=4, ensure_ascii=False, default=str)

    def recursive_check(self, file_dir, type: str = "cpp", log_dir="./"):
        if not os.path.isdir(file_dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        self.file_dir = file_dir
        self.log_dir = log_dir
        for root, _, files in os.walk(self.file_dir):
            if type == "make" and any(
                f in ["Makefile", "makefile", "GNUmakefile"] for f in files
            ):
                print(f"path: {root}")
                self._check(root, type)
            elif type == "cpp" and any(
                f.endswith(".c") or f.endswith(".cpp") for f in files
            ):
                print(f"path: {root}")
                self._check(root, type)

        self._write_record()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-fd",
        "--file_dir",
        metavar="",
        type=str,
        default="./",
        help="The directory to check the compilation",
    )
    parser.add_argument(
        "-ld",
        "--log_dir",
        metavar="",
        type=str,
        default="./",
        help="The directory to save the log",
    )
    parser.add_argument(
        "-t",
        "--type",
        metavar="",
        type=str,
        default="cpp",
        help="The type of the compilation, either cpp or make",
    )

    args = parser.parse_args()
    ckr = GenCodeChecker()
    ckr.recursive_check(args.file_dir, args.type, args.log_dir)
