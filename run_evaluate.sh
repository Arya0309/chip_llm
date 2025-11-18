#!/usr/bin/env bash
# ==================================================
# 自動化評估流程 (Evaluation Pipeline)
# ==================================================

set -euo pipefail

# ----------------------
# 參數設定 (可透過環境變數覆寫)
# ----------------------
RUNS=${OVERRIDE_RUNS:-4}
REF_ROUNDS=${OVERRIDE_ROUNDS:-5}
JOBS=${JOBS:-"$(nproc)"}
LOG_DIR=${OVERRIDE_LOG_DIR:-.log_gpt_summary_10_5}       # LOG 目錄
EVAL_ROOT_DEFAULT="data_eval"                             # 預設評估根目錄

# ----------------------
# 動態決定評估結果根目錄
# ----------------------
LOG_BASENAME=$(basename "${LOG_DIR}")
LOG_BASENAME_CLEAN="${LOG_BASENAME#.}"     
MODEL_NAME="${LOG_BASENAME_CLEAN/log_/}"   

if [[ "${MODEL_NAME}" != "${LOG_BASENAME_CLEAN}" ]] && [[ "${MODEL_NAME}" != "log" ]]; then
    EVAL_ROOT_DEFAULT="data_eval_${MODEL_NAME}"
fi

EVAL_ROOT=${OVERRIDE_EVAL_ROOT:-$EVAL_ROOT_DEFAULT}

RESULT_ROOT="${EVAL_ROOT}/result"
SUMMARY_ROOT="${EVAL_ROOT}/summary"
EVAL_OUT_DIR="${EVAL_ROOT}/evaluate"

mkdir -p "${RESULT_ROOT}" "${SUMMARY_ROOT}" "${EVAL_OUT_DIR}"

# ----------------------
# 1. 清理 build 目錄
# ----------------------
echo "清理 ${LOG_DIR} 下的 build 目錄..."
find "${LOG_DIR}" -type d -name build -print -exec rm -rf {} +
echo "清理完成。"
echo ""

# ----------------------
# 2. 執行 checker.py
# ----------------------
echo "執行 checker.py 統計每個 run/round..."

generate_tasks() {
    # 使用 -regex 確保匹配 run_X/round_Y
    find "${LOG_DIR}" -type d -regex ".*/run_[0-9]+/round_[0-9]+" \
        | sed -E "s#^${LOG_DIR}/run_([0-9]+)/round_([0-9]+)\$#\\1 \\2#" \
        | awk -v R="${RUNS}" -v RR="${REF_ROUNDS}" '{ if ($1>=1 && $1<=R && $2>=1 && $2<=RR) print $1, $2; }'
}

export RESULT_ROOT SUMMARY_ROOT LOG_DIR

# 使用 parallel 執行，每個 round 為一個任務
generate_tasks | parallel --will-cite -j "${JOBS}" --colsep ' ' '
i={1}; r={2};
run_dir="${LOG_DIR}/run_"$i
round_dir="$run_dir/round_"$r
res_dir="${RESULT_ROOT}/run_"$i
sum_dir="${SUMMARY_ROOT}/run_"$i
mkdir -p "$res_dir" "$sum_dir"

echo "Checking run_${i} (round_${r})"
if [ -d "$round_dir" ]; then
    python3 checker.py \
        --root "$round_dir" \
        --json "$res_dir/result_${r}.json" \
        --csv "$sum_dir/summary_${r}.csv" \
        --timeout 90
else
    echo "round_${r} missing, skip"
fi
'

echo "所有 checker.py 任務完成。"
echo ""

# ----------------------
# 3. 執行 evaluate.py
# ----------------------
echo "執行 evaluate.py"
python3 evaluate2.py \
    --runs "${RUNS}" \
    --max_rounds "${REF_ROUNDS}" \
    --summary_base "${SUMMARY_ROOT}" \
    --result_base "${RESULT_ROOT}" \
    --out_base "${EVAL_OUT_DIR}"
echo "evaluate.py 完成，結果請查閱 ${EVAL_OUT_DIR}/"
echo ""

echo "全部流程執行成功。"
