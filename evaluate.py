"""
逐 round 產出 evaluate/round_i_summary.csv，並畫 unit_test_pass_rate 與 pass@1/5/10 的圖
python3 evaluate.py --runs 20 --max_rounds 5

查單一 round 的平均（state 可用 raw 或 *_pass_rate）
python3 evaluate.py --round 3 --state unit_test_pass_rate

查單一 round 的 pass@k
python3 evaluate.py --round 3 --passk 5
"""

from __future__ import annotations

import argparse
import csv
import json
from pathlib import Path
from typing import Dict, List, Tuple, Iterable

import matplotlib.pyplot as plt
from matplotlib.ticker import PercentFormatter


# ---------------------------- I/O helpers ----------------------------

def _parse_summary_csv(path: Path) -> Dict[str, float]:
    """
    讀取一個 summary.csv，回傳 {state -> 數值}。
    - counts: 以整數儲存
    - *_pass_rate / unit_test_pass_rate: 以 [0,1] 浮點數儲存
    允許空白行；允許百分比字串。
    """
    data: Dict[str, float] = {}
    if not path.exists():
        return data
    with path.open("r", encoding="utf-8") as fp:
        reader = csv.reader(fp)
        header = next(reader, None)  # ['state','value']
        for row in reader:
            if not row or len(row) < 2:
                continue
            k, v = row[0].strip(), row[1].strip()
            if not k:
                continue
            if v.endswith("%"):
                try:
                    data[k] = float(v.strip("%")) / 100.0
                except Exception:
                    continue
            else:
                try:
                    data[k] = float(v)
                except Exception:
                    continue
    return data


def _load_results_json(path: Path) -> List[Dict]:
    if not path.exists():
        return []
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return []


# ---------------------------- Metrics ----------------------------

RAW_STATES = ["format_error", "compile_error", "runtime_error",
              "unit_test_fail", "unit_test_pass"]

PASS_RATE_STATES = ["format_pass_rate", "compile_pass_rate",
                    "runtime_pass_rate", "unit_test_pass_rate"]


def average_state_across_runs(
    round_id: int,
    state: str,
    *,
    runs: int = 20,
    summary_base: Path = Path("data_eval/summary")
) -> float:
    """
    讀取所有 run 在指定 round 的 summary.csv，計算指定 state 的平均。
    - 若 state 是 *_pass_rate（含 unit_test_pass_rate），回傳比率平均（0~1）
    - 若是 raw state（如 unit_test_pass），回傳數量平均
    缺檔會自動略過。
    """
    vals: List[float] = []
    for i in range(1, runs + 1):
        p = summary_base / f"run_{i}" / f"summary_{round_id}.csv"
        d = _parse_summary_csv(p)
        if state in d:
            vals.append(d[state])
    if not vals:
        return 0.0
    return sum(vals) / len(vals)


def pass_at_k_across_runs(
    round_id: int,
    k: int,
    *,
    runs: int = 20,
    result_base: Path = Path("data_eval/result")
) -> float:
    """
    對於同一 round，將 1..runs 視為 k 次嘗試的來源。
    以每題「前 k 次嘗試內至少 1 次 unit_test_pass」為 1，否則 0，
    再對所有題目取平均，得到該 round 的 pass@k。
    """
    # name -> list[bool] by run order
    per_problem: Dict[str, List[bool]] = {}

    for i in range(1, runs + 1):
        p = result_base / f"run_{i}" / f"result_{round_id}.json"
        items = _load_results_json(p)
        for it in items:
            name = it.get("name")
            st = it.get("state")
            if not name:
                continue
            per_problem.setdefault(name, [])
            # 對齊 run 序：確保每題都有固定長度
            per_problem[name].append(st == "unit_test_pass")

    if not per_problem:
        return 0.0

    total, hit = 0, 0
    for name, flags in per_problem.items():
        if not flags:
            continue
        kk = min(k, len(flags))
        total += 1
        if any(flags[:kk]):
            hit += 1

    return (hit / total) if total else 0.0


# ---------------------------- Plotting ----------------------------

def plot_over_rounds(
    series: Dict[int, float],
    *,
    title: str,
    ylabel: str = "Rate",
):
    """
    顯示折線圖（不存檔）。x=round, y=series[round] (0~1)。
    並以 round 1 的值畫一條水平基準線。
    """
    if not series:
        print(f"[plot] empty series for {title}")
        return

    rounds = sorted(series.keys())
    ys = [series[r] for r in rounds]
    y0 = series.get(1, ys[0])

    plt.figure()
    plt.plot(rounds, ys, marker="o")
    plt.axhline(y=y0, linestyle="--")
    plt.gca().yaxis.set_major_formatter(PercentFormatter(xmax=1))
    plt.title(title)
    plt.xlabel("Round")
    plt.ylabel(ylabel)
    plt.grid(True, linestyle=":")
    plt.show()


# ---------------------------- Main workflow ----------------------------

def build_round_summary_csv(
    round_id: int,
    *,
    runs: int,
    summary_base: Path,
    result_base: Path,
    out_dir: Path = Path("data_eval/evaluate"),
) -> Path:
    """
    將本 round 的「各 state 平均」與「pass@1/5/10」寫到
    data_eval/evaluate/round_{round_id}_summary.csv
    """
    out_dir.mkdir(parents=True, exist_ok=True)
    out_csv = out_dir / f"round_{round_id}_summary.csv"

    # 1) raw states 平均（count）
    state_avgs = {s: average_state_across_runs(round_id, s, runs=runs, summary_base=summary_base)
                  for s in RAW_STATES}

    # 2) pass rates 平均（0~1）
    rate_avgs = {s: average_state_across_runs(round_id, s, runs=runs, summary_base=summary_base)
                 for s in PASS_RATE_STATES}

    # 3) pass@k
    passk = {
        "pass@1": pass_at_k_across_runs(round_id, 1, runs=runs, result_base=result_base),
        "pass@5": pass_at_k_across_runs(round_id, 5, runs=runs, result_base=result_base),
        "pass@10": pass_at_k_across_runs(round_id, 10, runs=runs, result_base=result_base),
    }

    # 寫檔
    with out_csv.open("w", newline="", encoding="utf-8") as fp:
        w = csv.writer(fp)
        w.writerow(["state", "value"])
        for k, v in state_avgs.items():
            w.writerow([k, int(round(v))])  # counts 取整數較直觀
        w.writerow([])
        for k, v in rate_avgs.items():
            w.writerow([k, f"{v:.2%}"])
        w.writerow([])
        for k, v in passk.items():
            w.writerow([k, f"{v:.2%}"])

    return out_csv


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--runs", type=int, default=20)
    ap.add_argument("--max_rounds", type=int, default=5)
    ap.add_argument("--summary_base", default="data_eval/summary")
    ap.add_argument("--result_base", default="data_eval/result")
    ap.add_argument("--out_base", default="data_eval/evaluate")
    # 額外：單次查詢
    ap.add_argument("--round", type=int, help="僅對特定 round 計算（不指定則跑 1..max_rounds）")
    ap.add_argument("--state", type=str, help="僅計算該 state 的平均（搭配 --round 使用）")
    ap.add_argument("--passk", type=int, help="僅計算該 k 的 pass@k（搭配 --round 使用）")
    args = ap.parse_args()

    summary_base = Path(args.summary_base)
    result_base = Path(args.result_base)
    out_base = Path(args.out_base)

    # 功能 1 & 2：單 round 查詢（若指定）
    if args.round and args.state:
        avg = average_state_across_runs(args.round, args.state, runs=args.runs, summary_base=summary_base)
        print(f"[round {args.round}] avg({args.state}) = {avg:.6f}" if args.state.endswith("_rate") else
              f"[round {args.round}] avg({args.state}) = {avg:.0f}")
    if args.round and args.passk:
        pav = pass_at_k_across_runs(args.round, args.passk, runs=args.runs, result_base=result_base)
        print(f"[round {args.round}] pass@{args.passk} = {pav:.2%}")

    # 依需求：逐 round 產出 round_i_summary.csv，並畫圖
    rounds = [args.round] if args.round else list(range(1, args.max_rounds + 1))

    # 先建立各 round 的 summary.csv
    for r in rounds:
        out_csv = build_round_summary_csv(
            r, runs=args.runs, summary_base=summary_base, result_base=result_base, out_dir=out_base
        )
        print(f"[write] {out_csv}")

    # 畫圖（示例：unit_test_pass_rate & pass@1/5/10）
    # 你也可自行改為想看的 state（例如 compile_pass_rate）
    utr_series = {
        r: average_state_across_runs(r, "unit_test_pass_rate", runs=args.runs, summary_base=summary_base)
        for r in rounds
    }
    plot_over_rounds(utr_series, title="unit_test_pass_rate over rounds", ylabel="Rate")

    for k in (1, 5, 10):
        pk_series = {
            r: pass_at_k_across_runs(r, k, runs=args.runs, result_base=result_base)
            for r in rounds
        }
        plot_over_rounds(pk_series, title=f"pass@{k} over rounds", ylabel="Rate")


if __name__ == "__main__":
    main()
