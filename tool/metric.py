import os
import sys
import csv
import argparse
import pandas as pd
from math import comb

current_dir = os.path.dirname(os.path.abspath(__file__))
csv_file = "results.csv"


def compilation_success_rate(df, n):
    compilation_success = df.iloc[0]["compilation_success"]
    return compilation_success / n


def runtime_success_rate(df, n):
    runtime_success = df.iloc[0]["runtime_success"]
    return runtime_success / n


def pass_at_k(df, n, k):
    pass_at_k_values = []
    for _, row in df.iterrows():
        c = row["unit_test_success"]
        term = comb(n - c, k) / comb(n, k)
        pass_at_k_values.append(1 - term)

    return sum(pass_at_k_values) / len(pass_at_k_values)


def count_n(df):
    return (
        df.iloc[0]["compilation_error"]
        + df.iloc[0]["runtime_error"]
        + df.iloc[0]["unit_test_fail"]
        + df.iloc[0]["unit_test_success"]
    )


def load_each_round2df(path_template, num_files, num_rounds):
    list_df = [pd.DataFrame() for _ in range(num_rounds)]
    for i in range(num_files):
        df = pd.read_csv(path_template.format(i=i))
        for round_idx in range(len(df)):
            row = df.iloc[[round_idx]]
            list_df[round_idx] = pd.concat([list_df[round_idx], row], ignore_index=True)
    return list_df


def show_result(df, n, k, mode="wr"):
    csr = compilation_success_rate(df, n)
    rsr = runtime_success_rate(df, n)
    pass_k = pass_at_k(df, n, k)
    std = df["unit_test_success"].std()

    if "w" in mode:
        with open(csv_file, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([csr, rsr, pass_k, std])
    if "r" in mode:
        print()
        print(f"Compilation Success Rate: {csr:.3f}")
        print(f"Runtime Success Rate: {rsr:.3f}")
        print(f"Pass@{k}: {pass_k:.3f}")
        print(f"Standard Deviation: {std:.3f}")


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
        path_template = os.path.join(
            arg.path, ".temp_{i}/.log/compile_check_result.csv"
        )

        try:
            df_first = pd.read_csv(path_template.format(i=0))
        except FileNotFoundError:
            print("No cache files found.")
            sys.exit(0)

        num_questions = count_n(df_first)
        num_rounds = len(df_first)

        list_df = load_each_round2df(
            path_template,
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
                    "std",
                ]
            )
        for i in range(len(list_df)):
            show_result(list_df[i], num_questions, arg.k, "wr")

    else:
        df = pd.read_csv(arg.path)
        num_questions = count_n(df)
        print("Number of questions:", num_questions)
        print("Number of samples:  ", len(df))
        print()

        show_result(df, num_questions, arg.k, "r")
