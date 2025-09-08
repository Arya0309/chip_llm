export CUDA_VISIBLE_DEVICES=1


for i in $(seq 7 10); do
  python3 agents/agent_synth.py data_input_clear_IOR.json \
    -m openai/gpt-oss-20b \
    --max-iter 0 \
    --oa-model gpt-4o \
    --mode reflexion \
    -o ".log/gpt_oss_20_baseline_OA/gpt_oss_20_baseline_OA_${i}"
done
