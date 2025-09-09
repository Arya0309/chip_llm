export CUDA_VISIBLE_DEVICES=1


for i in $(seq 1 10); do
  python3 agents/agent_synth.py data_input_clear_IOR.json \
    -m openai/gpt-oss-20b \
    --mode reflexion \
    -o ".log/gpt_oss_20_SysPV2_wIOR_woStep0P_OA_${i}"
done
