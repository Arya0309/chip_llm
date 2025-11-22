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
from typing import Dict, List, Tuple, Iterable, Set
from math import comb

#import matplotlib.pyplot as plt
#from matplotlib.ticker import PercentFormatter


# ---------------------------- Constants ----------------------------

RAW_STATES = ["format_error", "compile_error", "runtime_error",
              "unit_test_fail", "unit_test_pass"]

PASS_RATE_STATES = ["format_pass_rate", "compile_pass_rate",
                    "runtime_pass_rate", "unit_test_pass_rate"]


# ---------------------------- Data Input Helpers ----------------------------

def _load_all_problem_names(data_input_path: Path) -> Tuple[Set[str], int]:
    """
    從 data_input.json 取得：
    - all_names: 全部題目名稱 (set)
    - total: 題目總數 (分母)
    支援兩種常見結構：
      1) JSON array，每個 element 帶 name
      2) JSON object，含 "elements": [...]
    """
    if not data_input_path.exists():
        raise FileNotFoundError(f"[data_input] not found: {data_input_path}")

    try:
        elems = json.loads(data_input_path.read_text(encoding="utf-8"))
    except Exception as e:
        raise RuntimeError(f"[data_input] invalid JSON: {data_input_path}: {e}")

    if not isinstance(elems, list):
        raise RuntimeError("[data_input] expect a list of elements or an object with key 'elements'")

    names: Set[str] = set()
    for it in elems:
        if isinstance(it, dict) and "name" in it and isinstance(it["name"], str):
            names.add(it["name"])

    total = len(names)
    if total == 0:
        raise RuntimeError("[data_input] empty names from data_input.json")

    return names, total


# ---------------------------- I/O helpers ----------------------------

def _parse_summary_csv_counts_only(path: Path) -> Dict[str, float]:
    """
    讀取一個 summary.csv，只取 raw counts（五個 RAW_STATES）。
    允許空白行；允許百分比字串（會被忽略，只保留可轉成數字的項）。
    缺項目視為 0。
    """
    counts: Dict[str, float] = {k: 0.0 for k in RAW_STATES}
    if not path.exists():
        return counts

    with path.open("r", encoding="utf-8") as fp:
        reader = csv.reader(fp)
        header = next(reader, None)  # e.g. ['state','value']
        for row in reader:
            if not row or len(row) < 2:
                continue
            k, v = row[0].strip(), row[1].strip()
            if k in RAW_STATES:
                # 優先以數字解讀
                try:
                    if v.endswith("%"):
                        # 百分比不是 counts，忽略
                        continue
                    counts[k] = float(v)
                except Exception:
                    # 讀不到就忽略，維持 0
                    pass
    return counts


def _load_results_json(path: Path) -> List[Dict]:
    if not path.exists():
        return []
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return []


def _load_results_with_backfill(path: Path, all_names: Set[str]) -> List[Dict]:
    """
    載入 result_{round}.json，若缺少某些題目，補成 unit_test_pass。
    回傳 list[{"name": str, "state": str, ...}]（原欄位保留）。
    """
    items = _load_results_json(path)
    seen = set()
    out: List[Dict] = []
    for it in items:
        name = it.get("name")
        if isinstance(name, str) and name:
            seen.add(name)
            out.append(it)
    missing = all_names - seen
    for name in missing:
        out.append({"name": name, "state": "unit_test_pass"})
    return out


# ---------------------------- Summary normalization ----------------------------

def _normalize_summary(counts: Dict[str, float], total: int) -> Dict[str, float]:
    """
    修正 raw counts 若總和 < total，將差額加到 unit_test_pass，
    並據此重新計算 *_pass_rate。
    回傳包含 counts 與 pass_rate 的完整 dict（比率為 [0,1] 浮點）。
    """
    # 1) 先確保 key 都存在
    for k in RAW_STATES:
        counts.setdefault(k, 0.0)

    # 2) 若總和 < total，把差額補到 unit_test_pass
    raw_sum = sum(float(counts[k]) for k in RAW_STATES)
    if raw_sum < total:
        counts["unit_test_pass"] = float(counts["unit_test_pass"]) + (total - raw_sum)
    # 若 raw_sum > total，保持原樣（通常表示來源統計邏輯有誤，但避免負補）

    # 3) 重新計算 pass rates（以 TOTAL 為分母）
    fmt_err = float(counts["format_error"])
    cmp_err = float(counts["compile_error"])
    run_err = float(counts["runtime_error"])
    ut_fail = float(counts["unit_test_fail"])
    ut_pass = float(counts["unit_test_pass"])

    # 累積觀念的 pass rate
    fmt_pass_rate = max(0.0, min(1.0, 1.0 - (fmt_err / total)))
    cmp_pass_rate = max(0.0, min(1.0, 1.0 - ((fmt_err + cmp_err) / total)))
    run_pass_rate = max(0.0, min(1.0, 1.0 - ((fmt_err + cmp_err + run_err) / total)))
    ut_pass_rate  = max(0.0, min(1.0, ut_pass / total))

    out: Dict[str, float] = {k: float(counts[k]) for k in RAW_STATES}
    out["format_pass_rate"] = fmt_pass_rate
    out["compile_pass_rate"] = cmp_pass_rate
    out["runtime_pass_rate"] = run_pass_rate
    out["unit_test_pass_rate"] = ut_pass_rate
    return out


def _load_fixed_summary(path: Path, total: int) -> Dict[str, float]:
    counts = _parse_summary_csv_counts_only(path)
    return _normalize_summary(counts, total)


# ---------------------------- Metrics ----------------------------

def average_state_across_runs(
    round_id: int,
    state: str,
    *,
    runs: int = 20,
    summary_base: Path = Path("data_eval/summary"),
    data_input: Path = Path("data_input.json"),
) -> float:
    """
    讀取所有 run 在指定 round 的 summary_{round}.csv，先依 data_input.json 的 TOTAL 做修正，
    再取指定 state 的平均。
    """
    all_names, total = _load_all_problem_names(data_input)
    vals: List[float] = []
    for i in range(1, runs + 1):
        p = summary_base / f"run_{i}" / f"summary_{round_id}.csv"
        d = _load_fixed_summary(p, total)
        if state in d:
            vals.append(float(d[state]))
    if not vals:
        return 0.0
    return sum(vals) / len(vals)


def pass_at_k_across_runs(
    round_id: int,
    k: int,
    *,
    runs: int = 20,
    result_base: Path = Path("data_eval/result"),
    data_input: Path = Path("data_input.json"),
) -> float:
    """
    組合式估計器：
    - 對每題，將 1..runs 視為 n 次嘗試，其中有 c 次 unit_test_pass
    - pass@k = 平均_{題目} [ 1 - C(n - c, k) / C(n, k) ] （k>n 時取 k=n）
    - 這樣可保證 pass@1 = 平均 unit_test_pass_rate（與修正後 summary 一致）
    """
    all_names, total = _load_all_problem_names(data_input)
    n = runs
    kk = min(k, n)

    # 每題在 n 次嘗試中的通過次數 c
    pass_count: Dict[str, int] = {name: 0 for name in all_names}

    for i in range(1, runs + 1):
        p = result_base / f"run_{i}" / f"result_{round_id}.json"
        items = _load_results_with_backfill(p, all_names)
        for it in items:
            name = it.get("name")
            st = it.get("state")
            if name in pass_count and st == "unit_test_pass":
                pass_count[name] += 1

    if n <= 0:
        return 0.0

    denom = comb(n, kk) if kk <= n else 1
    if denom == 0:
        return 0.0

    acc = 0.0
    for name in all_names:
        c = pass_count[name]
        # 全 fail 的組合數 / 全組合數
        fail_all = comb(n - c, kk) if kk <= (n - c) else 0
        acc += 1.0 - (fail_all / denom)

    return acc / len(all_names) if all_names else 0.0


# ---------------------------- Plotting ----------------------------
"""
def plot_over_rounds(
    series: Dict[int, float],
    *,
    title: str,
    ylabel: str = "Rate",
):
    
    顯示折線圖（不存檔）。x=round, y=series[round] (0~1)。
    並以 round 1 的值畫一條水平基準線。
    
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
"""


# ---------------------------- Main workflow ----------------------------

def build_round_summary_csv(
    round_id: int,
    *,
    runs: int,
    summary_base: Path,
    result_base: Path,
    data_input: Path,
    out_dir: Path = Path("data_eval/evaluate"),
) -> Path:
    """
    將本 round 的「各 state 平均」與「pass@1/5/10」寫到
    data_eval/evaluate/round_{round_id}_summary.csv
    （平均之前已按 data_input.json 修正）
    """
    out_dir.mkdir(parents=True, exist_ok=True)
    out_csv = out_dir / f"round_{round_id}_summary.csv"

    # 1) raw states 平均（count）
    state_avgs = {
        s: average_state_across_runs(
            round_id, s, runs=runs, summary_base=summary_base, data_input=data_input
        )
        for s in RAW_STATES
    }

    # 2) pass rates 平均（0~1）
    rate_avgs = {
        s: average_state_across_runs(
            round_id, s, runs=runs, summary_base=summary_base, data_input=data_input
        )
        for s in PASS_RATE_STATES
    }

    # 3) pass@k（使用全集分母 + result backfill）
    # 3) pass@k（使用全集分母 + result backfill）
    passk = {}
    for k_val in [1, 5, 10]:
        if args.runs >= k_val:
            passk[f"pass@{k_val}"] = pass_at_k_across_runs(
                round_id, k_val, runs=runs, result_base=result_base, data_input=data_input
            )
        else:
            print(f"[skip] pass@{k_val} since runs={runs} < {k_val}")


    passk = {
        "pass@1": pass_at_k_across_runs(round_id, 1, runs=runs, result_base=result_base, data_input=data_input),
        "pass@5": pass_at_k_across_runs(round_id, 5, runs=runs, result_base=result_base, data_input=data_input),
        "pass@10": pass_at_k_across_runs(round_id, 10, runs=runs, result_base=result_base, data_input=data_input),
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
    ap.add_argument("--data_input", default="data_input.json")
    # 額外：單次查詢
    ap.add_argument("--round", type=int, help="僅對特定 round 計算（不指定則跑 1..max_rounds）")
    ap.add_argument("--state", type=str, help="僅計算該 state 的平均（搭配 --round 使用）")
    ap.add_argument("--passk", type=int, help="僅計算該 k 的 pass@k（搭配 --round 使用）")
    args = ap.parse_args()

    summary_base = Path(args.summary_base)
    result_base = Path(args.result_base)
    out_base = Path(args.out_base)
    data_input = Path(args.data_input)

    # 功能 1 & 2：單 round 查詢（若指定）
    if args.round and args.state:
        avg = average_state_across_runs(
            args.round, args.state, runs=args.runs, summary_base=summary_base, data_input=data_input
        )
        if args.state.endswith("_rate"):
            print(f"[round {args.round}] avg({args.state}) = {avg:.6f}")
        else:
            print(f"[round {args.round}] avg({args.state}) = {avg:.0f}")

    if args.round and args.passk:
        pav = pass_at_k_across_runs(
            args.round, args.passk, runs=args.runs, result_base=result_base, data_input=data_input
        )
        print(f"[round {args.round}] pass@{args.passk} = {pav:.2%}")

    # 依需求：逐 round 產出 round_i_summary.csv，並畫圖
    rounds = [args.round] if args.round else list(range(1, args.max_rounds + 1))

    # 先建立各 round 的 summary.csv
    for r in rounds:
        out_csv = build_round_summary_csv(
            r, runs=args.runs, summary_base=summary_base, result_base=result_base,
            data_input=data_input, out_dir=out_base
        )
        print(f"[write] {out_csv}")

    # 畫圖（示例：unit_test_pass_rate & pass@1/5/10）
    # utr_series = {
    #     r: average_state_across_runs(
    #         r, "unit_test_pass_rate", runs=args.runs, summary_base=summary_base, data_input=data_input
    #     )
    #     for r in rounds
    # }
    # plot_over_rounds(utr_series, title="unit_test_pass_rate over rounds", ylabel="Rate")

    # for k in (1, 5, 10):
    #     pk_series = {
    #         r: pass_at_k_across_runs(
    #             r, k, runs=args.runs, result_base=result_base, data_input=data_input
    #         )
    #         for r in rounds
    #     }
    #     plot_over_rounds(pk_series, title=f"pass@{k} over rounds", ylabel="Rate")


if __name__ == "__main__":
    main()
