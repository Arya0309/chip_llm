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
            data=[[0, 0, 0]],
            columns=["complete", "uncompilable", "unit_test_fail"],
            dtype=int,
        )
        self.recorder = []

    def _check(self, dir, type: str = "cpp"):
        # 檢查目錄是否存在
        if not os.path.isdir(dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        self._check_compilable(dir, type)

        if not self.compile_result.returncode == 0:
            self.table["uncompilable"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": False,
                    "unit_test_pass": False,
                    "error_msg": self.compile_result.stderr,
                }
            )
            return

        self._check_unit_test(dir)
        if not self.unit_test_result.returncode == 0:
            self.table["unit_test_fail"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": True,
                    "unit_test_pass": False,
                    "error_msg": self.unit_test_result.stderr,
                }
            )
            return

        self.table["complete"] += 1
        self.recorder.append(
            {
                "dir": dir,
                "compilable": True,
                "unit_test_pass": True,
                "error_msg": "",
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

        if result.returncode == 0:
            print(Fore.GREEN + "Compilation successful!")
        else:
            print(Fore.RED + "Compilation failed.")
        pass

    def _check_unit_test(self, dir):
        result = subprocess.run(
            ["./main"],
            cwd=dir,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        self.unit_test_result = result

        # 移除main檔案
        os.remove(os.path.join(dir, "main"))

        if result.returncode == 0:
            print(Fore.GREEN + "Unit test successful!")
        else:
            print(Fore.RED + "Unit test failed.")
        pass

    def _write_record(self, file_dir):
        self.table.to_csv(
            os.path.join(file_dir, "compile_check_result.csv"), index=False
        )
        print(self.table)

        for dict in self.recorder:
            if isinstance(dict["error_msg"], bytes):
                dict["error_msg"] = dict["error_msg"].decode("utf-8", "ignore")
        with open(
            os.path.join(file_dir, "compile_check_result.json"), "w", encoding="utf-8"
        ) as f:
            json.dump(self.recorder, f, indent=4, ensure_ascii=False, default=str)

    def recursive_check(self, file_dir, type: str = "cpp", log_dir="./"):
        if not os.path.isdir(file_dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        for root, _, files in os.walk(file_dir):
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

        self._write_record(log_dir)


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
