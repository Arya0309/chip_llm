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
                    "task": os.path.basename(dir),
                    "compilable": False,
                    "execution_pass": False,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.compile_result.stderr.decode("utf-8", "ignore"),
                    "generated_code": None,
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
                    "task": os.path.basename(dir),
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
                    "generated_code": None,
                }
            )
        elif test_status == "timeout":
            # 處理超時情況
            self.table["runtime_error"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "task": os.path.basename(dir),
                    "compilable": True,
                    "execution_pass": False,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": self.timeout_msg,
                    "generated_code": None,
                }
            )
        elif test_status == "unit_test_fail":
            # 執行成功但單元測試失敗
            self.table["runtime_success"] += 1
            self.table["unit_test_fail"] += 1

            out = self.unit_test_result.stdout.decode("utf-8", "ignore")

            err = self.unit_test_result.stderr.decode("utf-8", "ignore")
            _, sep, after = err.partition("Assertion")
            msg = out + sep + after

            self.recorder.append(
                {
                    "dir": dir,
                    "task": os.path.basename(dir),
                    "compilable": True,
                    "execution_pass": True,
                    "unit_test_pass": False,
                    "status": self.status,
                    "error_msg": msg,
                    "generated_code": None,
                }
            )
        elif test_status == "unit_test_success":
            # 執行與單元測試皆成功
            self.table["runtime_success"] += 1
            self.table["unit_test_success"] += 1
            self.recorder.append(
                {
                    "dir": dir,
                    "task": os.path.basename(dir),
                    "compilable": True,
                    "execution_pass": True,
                    "unit_test_pass": True,
                    "status": self.status,
                    "error_msg": None,
                    "generated_code": None,
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
                timeout=self.timeout,  # Increase if necessary, e.g., timeout=10
            )
        except subprocess.TimeoutExpired as e:
            print(Fore.RED + f"Timeout expired: {e}")
            self.timeout_msg = str(e)
            self.status = "timeout"
            # Return a specific status if you wish to differentiate a timeout
            return "timeout"

        self.unit_test_result = result

        # 移除 main 檔案
        try:
            os.remove(os.path.join(dir, "main"))
        except Exception:
            pass

        # 合併輸出內容
        output = result.stdout.decode("utf-8", "ignore") + result.stderr.decode(
            "utf-8", "ignore"
        )

        # 如果輸出中包含斷言失敗的相關字樣，視為單元測試失敗
        if "assert" in output.lower():
            print(Fore.RED + "Unit test fail.")
            self.status = "unit_test_fail"
            return "unit_test_fail"

        # 如果回傳碼不為 0，且不屬於斷言失敗，則視為運行時錯誤
        if result.returncode != 0:
            print(
                Fore.RED + "Runtime Error (return code: {})".format(result.returncode)
            )
            self.status = "runtime_error"
            return "runtime_error"

        # 回傳碼為 0 時，依據輸出判斷是否成功
        if "success" in output.lower():
            print(Fore.GREEN + "Unit test successful!")
            self.status = "unit_test_success"
            return "unit_test_success"
        else:
            print(Fore.RED + "Unit test failed.")
            print(output)
            self.status = "unit_test_fail"
            return "unit_test_fail"

    def _write_record(self):
        # Ensure the log directory exists
        os.makedirs(self.log_dir, exist_ok=True)

        # Create the full log path
        log_path = os.path.join(self.log_dir, ".log")
        os.makedirs(log_path, exist_ok=True)

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

    def recursive_check(self, file_dir, type: str = "cpp", log_dir="./", timeout=5):
        if not os.path.isdir(file_dir):
            print(Fore.RED + "Error: directory not exist")
            return False

        self.timeout = timeout
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
        "--type",
        metavar="",
        type=str,
        default="cpp",
        help="The type of the compilation, either cpp or make",
    )
    parser.add_argument(
        "-t",
        "--timeout",
        type=int,
        default=5,
        help="The timeout for the unit test",
    )

    args = parser.parse_args()
    ckr = GenCodeChecker()
    ckr.recursive_check(args.file_dir, args.type, args.log_dir, args.timeout)
