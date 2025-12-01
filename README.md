conda create -n chipllm python=3.10.18
conda activate chipllm

pip install uv
uv pip install vllm==0.10.2 --torch-backend=auto

# 流程
bash ./run_20.sh (需修改參數)
bash ./run_evaluate.sh (需修改參數)

# make
mkdir build
cd build
cmake ..
make        # 或 cmake --build .
./out

# tmux
tmux
Ctrl + b + d
tmux ls
tmux attach -t 0

# For A100 設定
BATCH=4

MAXTOK=16384

# Git 流程