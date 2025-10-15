#!/usr/bin/env bash
set -euo pipefail

RUNS=${RUNS:-1}
REF_ROUNDS=${REF_ROUNDS:-10}
JOBS=${JOBS:-"$(nproc)"}

RESULT_ROOT="data_eval/result"
SUMMARY_ROOT="data_eval/summary"
mkdir -p "${RESULT_ROOT}" "${SUMMARY_ROOT}"

# 1) 找出真的存在的 (run, round)
#    格式化成：「run_idx round_idx」
generate_tasks() {
  # 只在 .log/run_*/round_* 目錄存在時才輸出任務
  find .log -type d -regex '\.log/run_[0-9]+/round_[0-9]+' \
    | sed -E 's#^\.log/run_([0-9]+)/round_([0-9]+)$#\1 \2#' \
    | awk -v R="${RUNS}" -v RR="${REF_ROUNDS}" '
        { if ($1>=1 && $1<=R && $2>=1 && $2<=RR) print $1, $2; }
      '
}

export RESULT_ROOT SUMMARY_ROOT

# 2) 執行（每個 round 一個任務）
generate_tasks | parallel --will-cite -j "${JOBS}" --colsep ' ' '
i={1}; r={2};
run_dir=".log/run_"$i
round_dir="$run_dir/round_"$r
res_dir="${RESULT_ROOT}/run_"$i
sum_dir="${SUMMARY_ROOT}/run_"$i
mkdir -p "$res_dir" "$sum_dir"

echo ">>> Checking run_${i} (round_${r})"
if [ -d "$round_dir" ]; then
  python3 checker.py \
    --root "$round_dir" \
    --json "$res_dir/result_${r}.json" \
    --csv "$sum_dir/summary_${r}.csv"
else
  echo "    - round_${r} missing, skip"
fi
'

echo "All checks finished."
