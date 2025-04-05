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
    dataset: str, model=None, tokenizer=None, max_new_tokens=4096, batch_size=16
) -> pd.DataFrame:
    df = pd.read_json(os.path.join(dataset_dir, dataset, "data.json"), orient="records")
    df_input = pd.DataFrame(columns=["task", "prompt", "input"])

    for i, row in tqdm(df.iterrows(), desc="Generating Embeddings"):
        if row["adoption"] == True:
            prompt = row["prompt"]
            input = tokenizer.apply_chat_template(
                prompt, tokenize=False, add_generation_prompt=True
            )
            input = tokenizer(
                input, return_tensors="pt", padding=True, truncation=True
            ).to(next(model.parameters()).device)

            df_input = df_input._append(
                {
                    "task": row["task"],
                    "prompt": prompt,
                    "input": input,
                    "testbench": row["testbench"],
                    "module_name": row["module_name"],
                },
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

    for i in tqdm(range(0, len(df_input), batch_size), desc="Generating Responses"):
        batch = df_input[i : i + batch_size]
        batch_inputs = collate_inputs(batch["input"])
        with torch.no_grad():
            output = model.generate(**batch_inputs, max_new_tokens=max_new_tokens)
        for j in range(len(batch)):
            decoded_output = tokenizer.decode(output[j], skip_special_tokens=False)
            df_input.loc[i + j, "respond"] = response_extractor(decoded_output)
            df_input.loc[i + j, "output_code"] = (
                code_extractor(decoded_output, df_input.loc[i + j, "module_name"])
                + "\n\n"
                + df_input.loc[i + j, "testbench"]
            )

    return df_input


def response_extractor(response: str) -> str:
    pattern = r"<\|im_start\|>assistant(.*?)<\|im_end\|>"
    if re.search(pattern, response, re.DOTALL):
        matches = re.findall(pattern, response, re.DOTALL)
    else:
        print(response)
        raise Exception("Format Error: Missing assistant response.")
    return matches[0]


def code_extractor(response: str, true_module_name) -> str:
    pattern = r"```cpp(.*?)```"
    if re.search(pattern, response, re.DOTALL):
        matches = re.findall(pattern, response, re.DOTALL)
        # 過濾出包含 SystemC 代碼的 code snippet
        matches = [m for m in matches if "#include <systemc.h>" in m]
        if not matches:
            raise Exception("Format Error: Missing SystemC code snippet in response.")
        code_snippet = matches[0]

        modules = re.split(r"(SC_MODULE|int sc_main)", code_snippet)
        header = modules[0]
        modules = modules[1:]
        modules = [modules[i] + modules[i + 1] for i in range(0, len(modules), 2)]

        # Remove testbench and main
        modules = [
            module for module in modules if "sc_module(testbench)" not in module.lower()
        ]
        modules = [module for module in modules if "sc_main" not in module.lower()]

        # Change the name of the first module
        pattern = r"SC_MODULE\((.*?)\)"
        modules_name = re.findall(pattern, modules[0])
        modules_name = modules_name[0]
        if modules:
            code_snippet = header + "\n" + "\n".join(modules)
            code_snippet = code_snippet.replace(modules_name, true_module_name)
            return code_snippet
        else:
            raise Exception("Format Error: Missing modules.")
    else:
        print(response)
        raise Exception("Format Error: Missing code snippet.")


def output_dir_generator(df: pd.DataFrame, dataset: str):
    data_dir = os.path.join(dataset_dir, dataset)
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

    model_name = "Qwen/Qwen2.5-Coder-32B-Instruct"
    dataset = "geek"
    dtype = "float16"

    tokenizer = AutoTokenizer.from_pretrained(model_name)
    tokenizer.padding_side = "left"

    model = AutoModelForCausalLM.from_pretrained(
        model_name,
        torch_dtype=torch.float16,
        device_map="auto",
    )

    df_output = generate_systemc(dataset, model=model, tokenizer=tokenizer)
    output_dir_generator(df_output, dataset)
