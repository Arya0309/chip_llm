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
tmux attach -t 1

# git 分支
git stash push -m "將現有修改存為子分支的起始點"

git checkout -b new_sub_branch

git stash pop

git add .
git commit -m "在子分支上的初始提交：完成了某某功能"

# 
"FFT": "0.0%"

"Sobel_Edge_Detection": "0.0%"

"RotateMatrixElementsofaMatrix": "0.0%",
 
"MLP_FF": "0.0%",

"Median_Filter": "0.0%"