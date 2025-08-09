#!/usr/bin/env bash
set -euo pipefail

RUNS=20
REF_ROUNDS="${REF_ROUNDS:-5}"

JSON_PATH="data_input_new.json"
# MODEL="meta-llama/CodeLlama-34b-Instruct-hf"
# MODEL="openai/gpt-oss-20b"
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
    --batch_size "${BATCH}" \
    --temperature "${TEMP}" \
    --max_new_tokens "${MAXTOK}" \
    --round_id 1 \
    --refine_round_number "${REF_ROUNDS}" \
    -o "${out1}"

  # round_2 .. round_N
  echo ">>> Verifier for run_${i}  (max ${REF_ROUNDS} rounds)"
  python3 agents/agent_verify.py "${base}" \
    --max_rounds "${REF_ROUNDS}" \
    --temperature "${TEMP}" \
    --max_new_tokens "${MAXTOK}"
done

echo "All ${RUNS} runs finished."

# #!/usr/bin/env bash
# set -euo pipefail

# MODEL="openai/gpt-oss-20b" #  #meta-llama/CodeLlama-34b-Instruct-hf

# for i in {1..20}; do
#   out_dir="./.log/run_${i}"
#   echo ">>> Run ${i} -> ${out_dir}"
#   mkdir -p "${out_dir}" 
#   python3 agents/main_batch.py data_input_new.json \
#     --model "${MODEL}" \
#     --batch_size 16 \
#     --temperature 0.3 \
#     --max_new_tokens 4096 \
#     -o "${out_dir}"
# done

# echo "All 20 runs finished."

#!/usr/bin/env bash
# run_20.sh  —  先產生 round_1，再跑 verifier 做多輪 refine