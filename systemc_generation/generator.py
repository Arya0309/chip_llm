import os
import torch
import re
import examples
import pandas as pd
from tqdm import tqdm
import time
from transformers import AutoTokenizer, AutoModelForCausalLM
from torch.nn.utils.rnn import pad_sequence

current_dir = os.path.dirname(os.path.realpath(__file__))


def construct_prompt(code: str) -> str:

    prompt = """
    You will receive C++/C code as input. 

**Tasks**:
1. Read and thoroughly analyze the provided C++/C code.
2. Identify the input(s) and output(s) of the code.
3. Provide a detailed, line-by-line or block-by-block natural language summary that explains:
   - The inputs and outputs of the code.
   - Each major line or block of code and its functionality (e.g., declarations, loops, conditions, data transformations).
4. Finally, based on your analysis, generate an equivalent SystemC code snippet 
   that preserves the logic and can be used in a high-level synthesis flow.

**C++/C Code**:
```
{CODE}
```

**Instructions**:
- Your answer must explicitly list the inputs and outputs.
- Your answer must include a clear, line-by-line or block-by-block natural language explanation.
- After the summary, generate SystemC code that reflects the same behavior.
- Provide the SystemC code in a compilable snippet, for example:

### Example 1:
```
{FEW_SHOT_EXAMPLE_1}
```

### Example 2:
```
{FEW_SHOT_EXAMPLE_2}
```

Ensure that the SystemC code you provide faithfully captures the logic of the original C++/C source.
Make the explanation thorough and accessible.
"""

    prompt = prompt.format(
        CODE=code,
        FEW_SHOT_EXAMPLE_1=examples.structure_examples_1,
        FEW_SHOT_EXAMPLE_2=examples.structure_examples_2,
    )

    messages = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are a coding expert in C++/C code analysis and SystemC.",
        },
        {"role": "user", "content": prompt},
    ]

    return messages


def generate_systemc(
    dataset: str, model=None, tokenizer=None, max_new_tokens=4096, batch_size=8
) -> None:
    data_dir = os.path.join(current_dir, "dataset", dataset)
    df = pd.DataFrame(columns=["filename", "prompt", "input", "respond", "output_code"])
    for filename in tqdm(
        os.listdir(os.path.join(data_dir, "data")),
        desc="Generating embeddings",
    ):
        code_path = os.path.join(data_dir, "data", filename)
        with open(code_path, "r") as f:
            code = f.read()
        prompt = construct_prompt(code)
        input = tokenizer.apply_chat_template(
            prompt, tokenize=False, add_generation_prompt=True
        )
        input = tokenizer(input, return_tensors="pt", padding=True, truncation=True).to(
            next(model.parameters()).device
        )
        df = df._append(
            {"filename": filename.split(".")[0], "prompt": prompt, "input": input},
            ignore_index=True,
        )

    def collate_inputs(input_series):
        # 假設所有字典具有相同的鍵
        keys = input_series.iloc[0].keys()
        collated = {}
        for key in keys:
            # 先 squeeze 移除第一個維度，再進行 padding
            collated[key] = pad_sequence(
                [d[key].squeeze(0) for d in input_series],
                batch_first=True,
            )
        return collated

    for i in tqdm(range(0, len(df), batch_size), desc="Generating responses"):
        batch = df[i : i + batch_size]
        # 將 Series 合併成一個單一的字典
        batch_inputs = collate_inputs(batch["input"])
        with torch.no_grad():
            output = model.generate(**batch_inputs, max_new_tokens=max_new_tokens)
        for j in range(len(batch)):
            decoded_output = tokenizer.decode(output[j], skip_special_tokens=False)
            df.loc[i + j, "respond"] = response_extractor(decoded_output)
            df.loc[i + j, "output_code"] = code_extractor(decoded_output)

    for i, row in tqdm(df.iterrows(), desc="Writing output files"):
        filename = row["filename"]
        output_dir = os.path.join(data_dir, "output", filename)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        with open(os.path.join(data_dir, "output", filename, "main.cpp"), "w") as f:
            f.write(row["output_code"])
        with open(os.path.join(data_dir, "output", filename, "response.txt"), "w") as f:
            f.write(row["respond"])


def response_extractor(response: str) -> str:
    pattern = r"<\|im_start\|>assistant(.*?)<\|im_end\|>"
    if re.search(pattern, response, re.DOTALL):
        matches = re.findall(pattern, response, re.DOTALL)
    else:
        print(response)
        raise Exception("Format Error: Missing assistant response.")
    return matches[0]


def code_extractor(response: str) -> str:
    pattern = r"```cpp(.*?)```"
    if re.search(pattern, response, re.DOTALL):
        matches = re.findall(pattern, response, re.DOTALL)
        matches = [m for m in matches if "#include <systemc.h>" in m]
        if not matches:
            raise Exception("Format Error: Missing SystemC code snippet in response.")
    else:
        print(response)
        raise Exception("Format Error: Missing code snippet.")
    return matches[0]


if __name__ == "__main__":
    tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-32B-Instruct")
    tokenizer.padding_side = "left"

    model = AutoModelForCausalLM.from_pretrained(
        "Qwen/Qwen2.5-Coder-32B-Instruct",
        torch_dtype=torch.float16,
        device_map="auto",
    )

    generate_systemc(
        "geek", model=model, tokenizer=tokenizer, max_new_tokens=4096, batch_size=16
    )
