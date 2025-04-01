import os
import subprocess
from colorama import init, Fore
import argparse
import pandas as pd
import json

init(autoreset=True)


class GenCodeChecker:
    def __init__(self):
        # 初始化新統計欄位
        self.table = pd.DataFrame(
            data=[[0, 0, 0, 0, 0, 0]],
            columns=[
                "compilation_success",
                "compilation_error",
                "runtime_success",
                "runtime_error",
                "unit_test_success",
                "unit_test_fail",
            ],
            dtype=int,
        )
        self.recorder = []

    def _check(self, dir, type: str = "cpp"):
        # 檢查目錄是否存在
        if not os.path.isdir(dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        # 編譯檢查
        if not self._check_compilable(dir, type):
            # 更新編譯失敗統計
            self.table["compilation_error"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": False,
                    "execution_pass": False,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.compile_result.stdout.decode("utf-8", "ignore"),
                }
            )
            return

        # 編譯成功則更新統計
        self.table["compilation_success"] += 1

        # 執行及單元測試檢查
        test_status = self._check_unit_test(dir)
        if test_status == "runtime_error":
            self.table["runtime_error"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": True,
                    "execution_pass": False,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": (
                        self.unit_test_result.stdout.decode("utf-8", "ignore")
                        if self.unit_test_result
                        and isinstance(self.unit_test_result.stdout, bytes)
                        else ""
                    ),
                }
            )
        elif test_status == "unit_test_fail":
            # 執行成功但單元測試失敗
            self.table["runtime_success"] += 1
            self.table["unit_test_fail"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": True,
                    "execution_pass": True,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.unit_test_result.stdout.decode("utf-8", "ignore"),
                }
            )
        elif test_status == "unit_test_success":
            # 執行與單元測試皆成功
            self.table["runtime_success"] += 1
            self.table["unit_test_success"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "compilable": True,
                    "execution_pass": True,
                    "unit_test_pass": True,
                    "status": self.status,
                    "error_msg": self.unit_test_result.stdout.decode("utf-8", "ignore"),
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
            print(Fore.RED + "Compilation Error.")
            print(result.stdout.decode("utf-8", "ignore"))
            self.status = "compilation_error"
            return False
        else:
            print(Fore.GREEN + "Compilation Successful!")
            self.status = "compilation_success"
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
            # 若超時，將結果記錄為執行失敗
            self.unit_test_result = e
            print(Fore.RED + "Timeout during execution.")
            self.status = "runtime_error"
            return "runtime_error"

        output_stdout = result.stdout.decode("utf-8", "ignore")
        output_stderr = result.stderr.decode("utf-8", "ignore")
        output = output_stdout + output_stderr
        # 檢查是否有執行錯誤
        if "Error" in output:
            print(Fore.RED + "Runtime Error.")
            print(output)
            self.status = "runtime_error"
            return "runtime_error"
        else:
            # 執行成功
            if "success" in output.lower():
                print(Fore.GREEN + "Unit test successful!")
                self.status = "unit_test_success"
                return "unit_test_success"
            else:
                output = "Assertion" + output.split("Assertion")[1]
                print(Fore.RED + "Unit test failed.")
                print(output)
                self.status = "unit_test_fail"
                return "unit_test_fail"
        # 最後移除main檔案
        try:
            os.remove(os.path.join(dir, "main"))
        except Exception:
            pass

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
        for rec in self.recorder:
            if isinstance(rec["error_msg"], bytes):
                rec["error_msg"] = rec["error_msg"].decode("utf-8", "ignore")
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
