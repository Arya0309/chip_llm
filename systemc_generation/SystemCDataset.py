import os
import time
import pandas as pd
from torch.utils.data import Dataset
from vllm.inputs import TextPrompt, TokensPrompt
from debug_prompt import (
    prompt_compile_error,
    prompt_runtime_error,
    prompt_unit_test_error,
)

current_dir = os.path.dirname(os.path.realpath(__file__))


class SystemCDataset(Dataset):
    def __init__(self, path, tokenizer):
        super().__init__()
        self.tokenizer = tokenizer
        self.df = pd.read_json(path, orient="records")[
            [
                "task_id",
                "task",
                "testbench",
                "module_name",
                "prompt",
            ]
        ].dropna()

        self.df["rounded_prompt"] = None
        self.df["generated_code"] = ""
        self.df["done"] = False
        self.df["unit_test_pass"] = False
        self.df["status"] = ""
        self.df["error_msg"] = ""

        self._update_inputs()

    def _update_inputs(self):
        def build_chat_prompt(text):
            if isinstance(text, list):
                return self.tokenizer.apply_chat_template(
                    text, tokenize=False, add_generation_prompt=True
                )
            return str(text)

        if self.df["rounded_prompt"].isnull().any():
            self.df["input"] = self.df["prompt"].apply(lambda x: build_chat_prompt(x))
        else:
            self.df["input"] = self.df.apply(
                lambda x: x["prompt"] + x["rounded_prompt"], axis=1
            )
            self.df["input"] = self.df["input"].apply(lambda x: build_chat_prompt(x))

    def __getitem__(self, idx):
        item = self.df.iloc[idx]
        return {
            "task_id": item["task_id"],
            "task": item["task"],
            "testbench": item["testbench"],
            "module_name": item["module_name"],
            "prompt": item["prompt"],
            "input": item["input"],
        }

    def __len__(self):
        return len(self.df)

    def update_done(self, temp_dir, idx):
        file_dir = os.path.join(temp_dir, ".log", f"compile_check_result_{idx}.json")
        temp = pd.read_json(file_dir, orient="records")[
            [
                "task",
                "unit_test_pass",
                "status",
                "error_msg",
            ]
        ]

        # 1) 將"unit_test_pass, status, error_msg"的值更新到self.df中
        temp_mapping = temp.set_index("task")
        for col in ["unit_test_pass", "status", "error_msg"]:
            self.df[col] = self.df["task"].map(temp_mapping[col])

        # 2) 將"unit_test_pass"為True的task更新到done_task(list())中
        done_task = self.df[self.df["unit_test_pass"] == True]["task"].tolist()
        # 3) 並將self.df中"unit_test_pass"為True的task刪除
        self.df = self.df[self.df["unit_test_pass"] == False]

        def make_prompt(x):
            if len(x["error_msg"]) > 2000:  # 32768 token limit
                x["error_msg"] = x["error_msg"][:2000]
            status = x["status"]
            if status == "compile_error":
                return prompt_compile_error(x["generated_code"], x["error_msg"])
            elif status == "runtime_error":
                return prompt_runtime_error(x["generated_code"], x["error_msg"])
            else:
                return prompt_unit_test_error(x["generated_code"], x["error_msg"])

        # 4) 目前已經剩下尚未完成的task，從第二回合開始，都會將上次的結果+錯誤組成新的prompt
        self.df["rounded_prompt"] = self.df.apply(make_prompt, axis=1)
        # 5) 更新self.df["input"]的內容
        self._update_inputs()

        if self.df.empty:
            return True, done_task
        else:
            return False, done_task


def collate_fn(batch):
    task_ids = [b["task_id"] for b in batch]
    tasks = [b["task"] for b in batch]
    testbenches = [b["testbench"] for b in batch]
    module_names = [b["module_name"] for b in batch]
    inputs_raw = [b["input"] for b in batch]

    unified_inputs = []
    for raw in inputs_raw:
        # 1) token id list
        if isinstance(raw, list) and raw and isinstance(raw[0], int):
            unified_inputs.append(TokensPrompt(prompt_token_ids=raw))

        # 2) pure string
        elif isinstance(raw, str):
            unified_inputs.append(TextPrompt(prompt=raw))

        # 3) dict or list of dicts
        else:
            # dict with content
            if isinstance(raw, dict) and "content" in raw:
                text = str(raw["content"])
            # list: 强制把每一项都转成 str，NaN->""
            elif isinstance(raw, list):
                parts = []
                for elt in raw:
                    if isinstance(elt, dict) and "content" in elt:
                        parts.append(str(elt["content"]))
                    else:
                        parts.append("" if pd.isna(elt) else str(elt))
                text = "\n".join(parts)
            else:
                # 兜底
                text = str(raw)

            unified_inputs.append(TextPrompt(prompt=text))

    return {
        "task_id": task_ids,
        "task": tasks,
        "testbench": testbenches,
        "module_name": module_names,
        "prompt": [b["prompt"] for b in batch],
        "input": unified_inputs,
    }


if __name__ == "__main__":
    dataset = SystemCDataset(
        "/home/S113062615/chip_llm/systemc_generation/input_data/qwen/one_shot.json",
        tokenizer=None,
    )
    print(dataset.df.head())
