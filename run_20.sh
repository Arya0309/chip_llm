#!/usr/bin/env bash
set -euo pipefail

MODEL="openai/gpt-oss-20b" #  #meta-llama/CodeLlama-34b-Instruct-hf

for i in {1..20}; do
  out_dir="./.log/run_${i}"
  echo ">>> Run ${i} -> ${out_dir}"
  mkdir -p "${out_dir}" 
  python3 agents/main_batch.py data_input_new.json \
    --model "${MODEL}" \
    --batch_size 16 \
    --temperature 0.3 \
    --max_new_tokens 4096 \
    -o "${out_dir}"
done

echo "All 20 runs finished."
