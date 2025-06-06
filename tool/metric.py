import os
import sys
import csv
import json
import argparse
import pandas as pd
from math import comb

current_dir = os.path.dirname(os.path.abspath(__file__))
csv_file = "results.csv"


def compilation_success_rate(df, n):
    compilation_success = df["compilation_success"].sum() / len(df)
    return compilation_success / n


def runtime_success_rate(df, n):
    runtime_success = df["runtime_success"].sum() / len(df)
    return runtime_success / n


def init_df_pass_k(path):
    df = pd.DataFrame()
    with open(path, "r") as f:
        tasks = json.load(f)
        num_tasks = len(tasks)
        for task in tasks:
            new_row = [
                {
                    "task": task["task"],
                    "unit_test_pass": 0,
                }
            ]
            df = pd.concat([df, pd.DataFrame(new_row)], ignore_index=True)

    return df, num_tasks


def _pass_at_k(df, n, k):

    def helper(x):
        return 1 - comb(n - x, k) / comb(n, k)

    temp_df = df.copy()
    temp_df["pass_at_k"] = temp_df["unit_test_pass"].apply(helper)
    return temp_df["pass_at_k"].mean()


def pass_at_k(df_pass_k, num_budgets, k, template):
    for i in range(num_budgets):
        file_path = template.format(i=i)
        with open(file_path, "r") as f:
            tasks = json.load(f)
            for task in tasks:
                df_pass_k.loc[
                    df_pass_k["task"] == task["task"], "unit_test_pass"
                ] += int(task["unit_test_pass"] is True)
    return _pass_at_k(df_pass_k, num_budgets, arg.k)


def load_each_round2df(path_template, num_files, num_rounds):
    list_df = [pd.DataFrame() for _ in range(num_rounds)]
    for i in range(num_files):
        df = pd.read_csv(path_template.format(i=i))
        for round_idx in range(len(df)):
            row = df.iloc[[round_idx]]
            list_df[round_idx] = pd.concat([list_df[round_idx], row], ignore_index=True)
    return list_df


def show_result(df, n, pass_at_k, k, mode="wr"):
    csr = compilation_success_rate(df, n)
    rsr = runtime_success_rate(df, n)
    # std = df["unit_test_success"].std()

    if "w" in mode:
        with open(csv_file, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(
                [
                    csr,
                    rsr,
                    pass_at_k,
                    # std,
                ]
            )
    if "r" in mode:
        print()
        print(f"Compilation Success Rate: {csr*100:.2f}")
        print(f"Runtime Success Rate: {rsr*100:.2f}")
        print(f"Pass@{k}: {pass_at_k*100:.2f}")
        # print(f"Standard Deviation: {std:.3f}")


if __name__ == "__main__":
    arg = argparse.ArgumentParser()
    arg.add_argument(
        "--path",
        "-p",
        type=str,
        required=True,
    )
    arg.add_argument(
        "--k",
        "-k",
        type=int,
        default=1,
    )

    arg = arg.parse_args()

    if "__cache__" in arg.path:
        path_template = {
            # pass_at_k
            "p": os.path.join(arg.path, ".temp_{i}/.log/compile_check_result_{j}.json"),
            # others
            "o": os.path.join(arg.path, ".temp_{i}/.log/compile_check_result.csv"),
        }
        df_pass_k, num_tasks = init_df_pass_k(path_template["p"].format(i=0, j=0))

        num_budgets = len(os.listdir(arg.path))
        num_rounds = len(os.listdir(os.path.join(arg.path, ".temp_0/.log")))
        print("Number of tasks:  ", num_tasks)
        print("Number of budgets:", num_budgets)
        print("Number of rounds: ", num_rounds)
        list_pass_k = []
        for j in range(num_rounds - 1):
            pass_k = pass_at_k(
                df_pass_k,
                num_budgets,
                arg.k,
                path_template["p"].format(i=0, j=j),
            )
            list_pass_k.append(pass_k)
            df_pass_k["unit_test_pass"] = 0

        try:
            df_first = pd.read_csv(path_template["o"].format(i=0))
        except FileNotFoundError:
            print("No cache files found.")
            sys.exit(0)

        num_rounds = len(df_first)

        list_df = load_each_round2df(
            path_template["o"],
            num_files=len(os.listdir(arg.path)),
            num_rounds=num_rounds,
        )

        with open(csv_file, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(
                [
                    "compilation_success_rate",
                    "runtime_success_rate",
                    f"pass@{arg.k}",
                    # "std",
                ]
            )
        for i in range(len(list_df)):
            show_result(list_df[i], num_tasks, list_pass_k[i], arg.k, "wr")

    else:
        arg.path = os.path.join(arg.path, ".log")
        path_template = os.path.join(arg.path, "compile_check_result_{i}.json")

        df_pass_k, num_tasks = init_df_pass_k(path_template.format(i=0))

        num_budgets = len(os.listdir(arg.path)) - 1
        pass_k = pass_at_k(df_pass_k, num_budgets, arg.k, path_template)

        arg.path = os.path.join(arg.path, "compile_check_result.csv")
        df_others = pd.read_csv(arg.path)
        print("Number of tasks:", num_tasks)
        print("Number of budgets:  ", num_budgets)
        show_result(df_others, num_tasks, pass_k, arg.k, "r")
