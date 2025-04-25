import pandas as pd
from torch.utils.data import Dataset
from vllm.inputs import TextPrompt, TokensPrompt


class SystemCDataset(Dataset):
    def __init__(self, path, tokenizer):
        super().__init__()
        self.df = pd.read_json(path, orient="records")[
            [
                "task_id",
                "task",
                "testbench",
                "module_name",
                "prompt",
            ]
        ].dropna()
        try:
            self.df["input"] = tokenizer.apply_chat_template(
                self.df.prompt.tolist(), tokenize=False, add_generation_prompt=True
            )
        except Exception as e:
            print("Chat template not supported, using default template")
            self.df["input"] = self.df.prompt.tolist()

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


def collate_fn(batch):
    task_ids = [b["task_id"] for b in batch]
    tasks = [b["task"] for b in batch]
    testbenches = [b["testbench"] for b in batch]
    module_names = [b["module_name"] for b in batch]
    inputs_raw = [b["input"] for b in batch]

    unified_inputs = []
    for raw in inputs_raw:
        # 1) 如果已经是 token id 列表 -> 用 TokensPrompt
        if isinstance(raw, list) and raw and isinstance(raw[0], int):
            unified_inputs.append(TokensPrompt(prompt_token_ids=raw))

        # 2) 如果是纯字符串 -> 直接用 TextPrompt（也可以不包，vLLM 也接受 str）
        elif isinstance(raw, str):
            unified_inputs.append(TextPrompt(prompt=raw))

        # 3) 如果是 dict（例如 {"role":"user","content":...}）或者 list of dicts
        else:
            # 尝试抽取 content 字段拼成一条文本
            if isinstance(raw, dict) and "content" in raw:
                text = raw["content"]
            elif isinstance(raw, list):
                parts = []
                for elt in raw:
                    if isinstance(elt, dict) and "content" in elt:
                        parts.append(elt["content"])
                    else:
                        parts.append(str(elt))
                text = "\n".join(parts)
            else:
                # 兜底：转成字符串
                text = str(raw)

            unified_inputs.append(TextPrompt(prompt=text))

    return {
        "task_id": task_ids,
        "task": tasks,
        "testbench": testbenches,
        "module_name": module_names,
        "prompt": [item["prompt"] for item in batch],
        "input": unified_inputs,
    }
