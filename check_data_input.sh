#!/bin/bash

# 設定輸入資料夾與輸出檔案名稱
INPUT_DIR="data_inputs_new"
OUTPUT_FILENAME="final.txt"

# 取得目前腳本所在的絕對路徑，確保切換目錄後還能找到 final.txt
BASE_DIR=$(pwd)
OUTPUT_FILE="$BASE_DIR/$OUTPUT_FILENAME"

# 1. 初始化：清空或建立新的 final.txt
> "$OUTPUT_FILE"

echo "開始編譯、執行並比對答案..."

# 2. 迴圈：遍歷 data_input 底下的所有子資料夾
for folder in "$INPUT_DIR"/*; do
    if [ -d "$folder" ]; then
        
        # 取得資料夾名稱 (例如: problem_01)
        folder_name=$(basename "$folder")
        
        # 為了確保 C++ 產出的 ans.txt 在該資料夾內，我們先切換進去
        cd "$folder" || continue

        # 尋找當前資料夾內的 .cpp 檔案
        cpp_file=$(find . -maxdepth 1 -name "*.cpp" | head -n 1)

        if [ -n "$cpp_file" ]; then
            echo "---------------------------------"
            echo "正在處理: $folder_name"
            
            exe_file="./temp_exe"
            ans_file="ans.txt"       # 程式自己會產出這個檔名
            golden_file="golden.txt" # 標準答案

            # 3. 編譯
            g++ "$cpp_file" -o "$exe_file"

            if [ $? -eq 0 ]; then
                
                # 4. 執行
                # 這裡移除了 "> $ans_file"，因為您的程式會自己寫檔
                "$exe_file"
                
                # 5. 比對 ans.txt 與 golden.txt
                # 先檢查 ans.txt 是否真的被程式產生了
                if [ -f "$ans_file" ]; then
                    
                    if [ -f "$golden_file" ]; then
                        # 使用 awk 比對正確行數
                        correct_count=$(awk 'NR==FNR{a[NR]=$0; next} $0==a[FNR]{c++} END{print c+0}' "$golden_file" "$ans_file")
                        total_lines=$(wc -l < "$golden_file" | tr -d ' ')
                        
                        echo " -> 比對結果: 正確 $correct_count 行 / 共 $total_lines 行"
                        
                        # 寫入比對結果到 final.txt
                        echo "---------------------------------" >> "$OUTPUT_FILE"
                        echo "題目: $folder_name (正確率: $correct_count/$total_lines)" >> "$OUTPUT_FILE"
                        echo "---------------------------------" >> "$OUTPUT_FILE"
                    else
                        echo " -> [警告] 找不到 golden.txt，無法比對"
                        echo "---------------------------------" >> "$OUTPUT_FILE"
                        echo "題目: $folder_name (無 golden.txt)" >> "$OUTPUT_FILE"
                        echo "---------------------------------" >> "$OUTPUT_FILE"
                    fi

                    # 將 ans.txt 的內容附加到總表 final.txt
                    cat "$ans_file" >> "$OUTPUT_FILE"
                    echo "" >> "$OUTPUT_FILE"
                    
                else
                    echo " -> [錯誤] 程式執行完畢，但未發現 $ans_file，請檢查 C++ 寫檔邏輯"
                    echo "題目: $folder_name (未產出 ans.txt)" >> "$OUTPUT_FILE"
                fi

                # 6. 清理：刪除執行檔
                rm "$exe_file"
            else
                echo "[錯誤] $folder_name 編譯失敗"
                echo "題目: $folder_name (編譯失敗)" >> "$OUTPUT_FILE"
            fi
        else
            echo "[警告] 在 $folder_name 中找不到 .cpp 檔案"
        fi

        # 處理完一個資料夾後，切回原本的根目錄，準備進入下一個迴圈
        cd "$BASE_DIR"
    fi
done

echo "全部完成！總結果已儲存至 $OUTPUT_FILENAME"