#!/usr/bin/env bash
set -euo pipefail

for i in {1..20}; do
  run_dir="./.log/run_${i}"
  # run_dir="./data_eval/Qwen32/.log/run_${i}"
  if [[ ! -d "$run_dir" ]]; then
    echo "Skip: $run_dir 不存在"
    continue
  fi

  out_dir="./data_eval"
  mkdir -p "$out_dir"

  echo ">>> Checking ${run_dir}"
  python3 checker.py \
    --root "${run_dir}" \
    --timeout 5.0 \
    --json "${out_dir}/results_${i}.json" \
    --csv  "${out_dir}/summary_${i}.csv"
done

echo "All 20 runs checked."
