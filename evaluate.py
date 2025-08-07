#!/usr/bin/env python3
import argparse
import csv
import glob
import os
import sys
from typing import Dict, List

DEFAULT_STATES = ["format_error", "compile_error", "unit_test_fail", "unit_test_pass"]

def parse_args():
    ap = argparse.ArgumentParser()
    ap.add_argument("--dir", default="data_eval", help="包含 summary_*.csv 的資料夾")
    ap.add_argument("--pattern", default="summary_*.csv", help="CSV 檔案樣式")
    ap.add_argument("--divisor", type=float, default=31.0, help="最後要除的數（預設 31）")
    ap.add_argument(
        "--states",
        nargs="*",
        default=DEFAULT_STATES,
        help="要統計的 state 名稱列表（預設四種）",
    )
    ap.add_argument("--precision", type=int, default=4, help="輸出小數位數")
    return ap.parse_args()

def read_counts(path: str, states: List[str]) -> Dict[str, float]:
    """讀一個 summary_i.csv，回傳 {state: count}；缺的 state 補 0。"""
    per = {s: 0.0 for s in states}
    with open(path, "r", encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        if "state" not in reader.fieldnames or "count" not in reader.fieldnames:
            raise ValueError(f"{path} 缺少 'state' 或 'count' 欄位")
        for row in reader:
            s = (row.get("state") or "").strip()
            if s in per:
                try:
                    c = float(row.get("count", 0) or 0)
                except ValueError:
                    c = 0.0
                per[s] += c
    return per

def main():
    args = parse_args()
    pattern = os.path.join(args.dir, args.pattern)
    files = sorted(glob.glob(pattern))
    if not files:
        print(f"找不到任何 CSV：{pattern}", file=sys.stderr)
        sys.exit(1)

    # 累積每個檔案的各 state 計數，之後算平均
    collected: Dict[str, List[float]] = {s: [] for s in args.states}

    for fp in files:
        per = read_counts(fp, args.states)
        for s in args.states:
            collected[s].append(per.get(s, 0.0))

    n = len(files)
    print("state, average_count, average_normalized")

    for s in args.states:
        mean_count = sum(collected[s]) / n if n > 0 else 0.0
        normalized = mean_count / args.divisor if args.divisor != 0 else float("nan")
        print(f"{s}, {mean_count:.{args.precision}f}, {normalized:.{args.precision}f}")

if __name__ == "__main__":
    main()
