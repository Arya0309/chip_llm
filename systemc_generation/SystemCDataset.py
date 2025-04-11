import pandas as pd
from torch.utils.data import Dataset


class SystemCDataset(Dataset):
    def __init__(self, path, tokenizer):
        super().__init__()
        self.df = pd.read_json(path, orient="records")[
            ["task_id", "testbench", "module_name", "prompt"]
        ].dropna()
        self.df["input"] = tokenizer.apply_chat_template(
            self.df.prompt.tolist(), tokenize=False, add_generation_prompt=True
        )

    def __getitem__(self, idx):
        item = self.df.iloc[idx]
        return {
            "task_id": item["task_id"],
            "testbench": item["testbench"],
            "module_name": item["module_name"],
            "prompt": item["prompt"],
            "input": item["input"],
        }

    def __len__(self):
        return len(self.df)


def collate_fn(batch):
    return {
        "task_id": [item["task_id"] for item in batch],
        "testbench": [item["testbench"] for item in batch],
        "module_name": [item["module_name"] for item in batch],
        "prompt": [item["prompt"] for item in batch],
        "input": [item["input"] for item in batch],
    }
