#!/usr/bin/env bash
set -euo pipefail

RUNS=20
REF_ROUNDS="${REF_ROUNDS:-5}"

RESULT_ROOT="data_eval/result"
SUMMARY_ROOT="data_eval/summary"

mkdir -p "${RESULT_ROOT}" "${SUMMARY_ROOT}"

for i in $(seq 1 "${RUNS}"); do
  run_dir=".log/run_${i}"
  echo ">>> Checking run_${i} (${REF_ROUNDS} rounds)"

  res_dir="${RESULT_ROOT}/run_${i}"
  sum_dir="${SUMMARY_ROOT}/run_${i}"
  mkdir -p "${res_dir}" "${sum_dir}"

  for r in $(seq 1 "${REF_ROUNDS}"); do
    round_dir="${run_dir}/round_${r}"
    if [[ ! -d "${round_dir}" ]]; then
      echo "  - round_${r} missing, skip"
      continue
    fi

    # 正確呼叫 checker.py：使用 --root / --json / --csv
    python3 checker.py \
      --root "${round_dir}" \
      --json "${res_dir}/result_${r}.json" \
      --csv "${sum_dir}/summary_${r}.csv"
  done
done

echo "All checks finished."

# #!/usr/bin/env bash
# set -euo pipefail

# for i in {1..20}; do
#   run_dir="./.log/run_${i}"
#   # run_dir="./data_eval/Qwen32/.log/run_${i}"
#   if [[ ! -d "$run_dir" ]]; then
#     echo "Skip: $run_dir 不存在"
#     continue
#   fi

#   out_dir="./data_eval"
#   mkdir -p "$out_dir"

#   echo ">>> Checking ${run_dir}"
#   python3 checker.py \
#     --root "${run_dir}" \
#     --timeout 5.0 \
#     --json "${out_dir}/results_${i}.json" \
#     --csv  "${out_dir}/summary_${i}.csv"
# done

# echo "All 20 runs checked."

#!/usr/bin/env bash
# run_20_check.sh — 逐 round 測試並彙總結果到 data_eval/result & summary