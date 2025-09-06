#!/usr/bin/env bash
set -euo pipefail


# metric.py 的執行命令
METRIC_CMD="python3 ./tool/metric.py"

# 執行 metric.py
echo "Running Qwen-7B_sd pass@1..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sd" \
    -k 1

echo "Running Qwen-7B_sd pass@5..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sd" \
    -k 5

echo "Running Qwen-7B_sd pass@10..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sd" \
    -k 10

echo "Running Qwen-7B_sr pass@1..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sr" \
    -k 1
echo "Running Qwen-7B_sr pass@5..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sr" \
    -k 5
echo "Running Qwen-7B_sr pass@10..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/7B_sr" \
    -k 10
echo "Running Qwen-14B_sd pass@1..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sd" \
    -k 1
echo "Running Qwen-14B_sd pass@5..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sd" \
    -k 5
echo "Running Qwen-14B_sd pass@10..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sd" \
    -k 10
echo "Running Qwen-14B_sr pass@1..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sr" \
    -k 1
echo "Running Qwen-14B_sr pass@5..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sr" \
    -k 5
echo "Running Qwen-14B_sr pass@10..."
micromamba run -n chip_llm $METRIC_CMD \
    -p "/home/S113062628/project/chip_llm/chip_llm/reg/Qwen/14B_sr" \
    -k 10
