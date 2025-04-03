import os
import torch
import re
import examples
import pandas as pd
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM
from torch.nn.utils.rnn import pad_sequence

current_dir = os.path.dirname(os.path.realpath(__file__))
dataset_dir = os.path.join(current_dir, "dataset")


def generate_systemc(
    dataset: str, model=None, tokenizer=None, max_new_tokens=4096, batch_size=8
) -> pd.DataFrame:
    df = pd.read_json(os.path.join(dataset_dir, dataset, "data.json"), orient="records")
    df_input = pd.DataFrame(columns=["task", "prompt", "input"])

    for i, row in tqdm(df.iterrows(), desc="Generating Embeddings"):
        prompt = row["prompt"]
        input = tokenizer.apply_chat_template(
            prompt, tokenize=False, add_generation_prompt=True
        )
        input = tokenizer(input, return_tensors="pt", padding=True, truncation=True).to(
            next(model.parameters()).device
        )

        df_input = df_input._append(
            {"task": row["task"], "prompt": prompt, "input": input},
            ignore_index=True,
        )

    def collate_inputs(input_series):
        keys = input_series.iloc[0].keys()
        collated = {}
        for key in keys:
            collated[key] = pad_sequence(
                [d[key].squeeze(0) for d in input_series],
                batch_first=True,
            )
        return collated

    for i in tqdm(range(0, len(df_input), batch_size), desc="Generating responses"):
        batch = df_input[i : i + batch_size]
        batch_inputs = collate_inputs(batch["input"])
        with torch.no_grad():
            output = model.generate(**batch_inputs, max_new_tokens=max_new_tokens)
        for j in range(len(batch)):
            decoded_output = tokenizer.decode(output[j], skip_special_tokens=False)
            df_input.loc[i + j, "respond"] = response_extractor(decoded_output)
            df_input.loc[i + j, "output_code"] = code_extractor(decoded_output)

    return df_input


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


def output_dir_generator(df: pd.DataFrame, data_dir: str):

    for i, row in tqdm(df.iterrows(), desc="Writing output files"):
        filename = row["task"]
        output_dir = os.path.join(data_dir, "output", filename)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        with open(os.path.join(data_dir, "output", filename, "main.cpp"), "w") as f:
            f.write(row["output_code"])
        with open(os.path.join(data_dir, "output", filename, "response.txt"), "w") as f:
            f.write(row["respond"])


if __name__ == "__main__":
    tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-32B-Instruct")
    tokenizer.padding_side = "left"

    model = AutoModelForCausalLM.from_pretrained(
        "Qwen/Qwen2.5-Coder-32B-Instruct",
        torch_dtype=torch.float16,
        device_map="auto",
    )

    dataset = "geek"

    df_output = generate_systemc(dataset, model=model, tokenizer=tokenizer)
    output_dir_generator(df_output, dataset)
