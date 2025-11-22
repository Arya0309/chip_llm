import json
import os
import re
import glob

# 設定檔案路徑
JSON_FILE = 'data_input_new_3.json'
INPUT_DIR = 'data_inputs_new'
OUTPUT_FILE = 'data_input_new_4.json'

def remove_comments(text):
    """
    移除 C/C++ 風格的註解 (// 和 /* */)
    """
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # 將註解替換為單個空格，避免黏合代碼
        else:
            return s
    
    # 正則表達式匹配： /*...*/ 或 //... 或 "字串"
    # 我們必須匹配字串以避免移除字串內部的 //
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

def minify_cpp(code):
    """
    技巧性壓縮 C++ 代碼以減少 Token
    """
    # 1. 移除註解
    code = remove_comments(code)
    
    # 2. 逐行處理
    lines = code.split('\n')
    cleaned_lines = []
    
    for line in lines:
        # 移除前後空白 (Trim)
        line = line.strip()
        
        # 3. 移除空行
        if not line:
            continue
            
        cleaned_lines.append(line)
    
    # 4. 重新組合成字串 (保留換行以維持 include 和 define 的正確性)
    # 如果要更極致，可以嘗試將非預處理指令接在一起，但風險較高，這裡保守僅移除縮排和空行
    return '\n'.join(cleaned_lines)

def main():
    # 讀取原始 JSON
    try:
        with open(JSON_FILE, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"錯誤: 找不到 {JSON_FILE}")
        return

    updated_count = 0

    for item in data:
        task_name = item.get('name')
        if not task_name:
            continue

        # 建構預期的資料夾路徑
        folder_path = os.path.join(INPUT_DIR, task_name)
        
        # 尋找該資料夾下的 .cpp 檔案
        cpp_files = glob.glob(os.path.join(folder_path, "*.cpp"))

        if not cpp_files:
            print(f"警告: 在 {folder_path} 中找不到 .cpp 檔案，跳過 {task_name}")
            continue
        
        # 假設每個題目只有一個主要的 cpp 檔，取第一個
        target_file = cpp_files[0]
        
        try:
            with open(target_file, 'r', encoding='utf-8') as f:
                raw_code = f.read()
            
            # *** 關鍵步驟：壓縮代碼 ***
            minified_code = minify_cpp(raw_code)
            
            # 替換 JSON 中的 code
            item['code'] = minified_code
            updated_count += 1
            print(f"成功更新: {task_name} (來源: {os.path.basename(target_file)})")
            
            # 顯示 Token 節省估算 (以字元數粗略估計)
            orig_len = len(raw_code)
            new_len = len(minified_code)
            saved_percent = (orig_len - new_len) / orig_len * 100
            print(f"  └─ 字元數從 {orig_len} 降至 {new_len} (節省約 {saved_percent:.1f}%)")

        except Exception as e:
            print(f"讀取檔案 {target_file} 時發生錯誤: {e}")

    # 寫入新的 JSON 檔案
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
        json.dump(data, f, indent=4, ensure_ascii=False)

    print(f"\n完成！已更新 {updated_count} 筆資料，結果存於 {OUTPUT_FILE}")

if __name__ == "__main__":
    main()