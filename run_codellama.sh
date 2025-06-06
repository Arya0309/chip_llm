#!/usr/bin/env bash
set -euo pipefail

# Path to your config directory
CONFIG_DIR="./systemc_generation/config"


echo "Running CodeLlama-7b-Instruct-hf..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/CodeLlama-7b-Instruct-hf.yaml"

echo "Running CodeLlama-13b-Instruct-hf..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/CodeLlama-13b-Instruct-hf.yaml"

echo "Running CodeLlama-34b-Instruct-hf..."
micromamba run -n chip_llm python3 ./systemc_generation/vllm_generator.py \
    --config_path "$CONFIG_DIR/CodeLlama-34b-Instruct-hf.yaml"