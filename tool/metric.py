import os
import argparse
import pandas as pd
from math import comb

current_dir = os.path.dirname(os.path.abspath(__file__))

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

df = pd.read_csv(arg.path)
num_samples = (
    df.iloc[0]["compilation_error"]
    + df.iloc[0]["runtime_error"]
    + df.iloc[0]["unit_test_fail"]
    + df.iloc[0]["unit_test_success"]
)
print("Number of samples:", num_samples)
print("Number of budget: ", len(df))
print()


def compilation_success_rate(df):
    compilation_success = df.iloc[0]["compilation_success"]
    return compilation_success / num_samples


def runtime_success_rate(df):
    runtime_success = df.iloc[0]["runtime_success"]
    return runtime_success / num_samples


def pass_at_k(df, k):
    pass_at_k_values = []
    for _, row in df.iterrows():
        c = row["unit_test_success"]
        term = comb(num_samples - c, k) / comb(num_samples, k)
        pass_at_k_values.append(1 - term)

    return sum(pass_at_k_values) / len(pass_at_k_values)


print(f"{'Compilation success rate:':<25} {compilation_success_rate(df):.3f}")
print(f"{'Runtime success rate:':<25} {runtime_success_rate(df):.3f}")
print(f"{f'Pass@{arg.k}:':<25} {pass_at_k(df, arg.k):.3f}")
