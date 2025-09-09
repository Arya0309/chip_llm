#!/usr/bin/env bash
set -euo pipefail

RUNS=20
REF_ROUNDS="${REF_ROUNDS:-5}"

JSON_PATH="data_input_new.json"
# MODEL="meta-llama/CodeLlama-34b-Instruct-hf"
MODEL="openai/gpt-oss-20b"
BATCH=16
TEMP=0.3
# TOPP=0.8
MAXTOK=8192

mkdir -p ".log"

for i in $(seq 1 "${RUNS}"); do
  base=".log/run_${i}"
  out1="${base}/round_1"
  echo ">>> Run ${i}  (round_1) -> ${out1}"
  mkdir -p "${out1}"

  # round_1
  python3 agents/main_batch.py "${JSON_PATH}" \
    --model "${MODEL}" \
    --batch_size "${BATCH}" \
    --temperature "${TEMP}" \
    --max_new_tokens "${MAXTOK}" \
    --round_id 1 \
    --refine_round_number "${REF_ROUNDS}" \
    -o "${out1}"

  # round_2 .. round_N
  if [ "${REF_ROUNDS}" -gt 1 ]; then
  echo ">>> Verifier for run_${i}  (max ${REF_ROUNDS} rounds)"
  python3 agents/agent_verify.py "${base}" \
    --max_rounds "${REF_ROUNDS}" \
    --temperature "${TEMP}" \
    --max_new_tokens "${MAXTOK}"
  fi
done

echo "All ${RUNS} runs finished."