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