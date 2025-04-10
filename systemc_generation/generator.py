import os
import torch
import re
import warnings


import pandas as pd
from tqdm import tqdm


from transformers import AutoTokenizer, AutoModelForCausalLM
from torch.nn.utils.rnn import pad_sequence


current_dir = os.path.dirname(os.path.realpath(__file__))
dataset_dir = os.path.join(current_dir, "dataset")


def generate_systemc(
    dataset: str, model=None, tokenizer=None, max_new_tokens=4096, batch_size=16
) -> pd.DataFrame:
    """
    Processes a JSON-lines dataset to generate model inputs for a SystemC generation task.

    This function reads a JSON-lines dataset file containing test cases for generating SystemC modules.
    For each entry in the dataset where the "adoption" flag is True, the function:
      1. Extracts the prompt.
      2. Formats the prompt using the tokenizer's 'apply_chat_template' method, optionally adding a generation prompt.
      3. Tokenizes the formatted prompt into a tensor, ensuring it is moved to the same device as the model's parameters.
      4. Constructs a DataFrame that aggregates the task, the original prompt, the tokenized input,
         the testbench data, and the module name.

    Parameters:
        dataset (str): The filename of the dataset JSON-lines file. The file path is constructed using a predefined 'dataset_dir'.
        model: The model instance used for processing inputs. The device of the first parameter of the model is used to place tensors.
        tokenizer: The tokenizer that includes a method to apply a chat template ('apply_chat_template') and can tokenize text.
        max_new_tokens (int, optional): The maximum number of tokens to generate (default is 4096).
        batch_size (int, optional): The number of samples to process at a time (default is 16).

    Returns:
        pd.DataFrame: A DataFrame with the following columns:
            - "task": The description of the task as provided in the dataset.
            - "prompt": The original prompt from the dataset.
            - "input": The tokenized input tensor, ready for processing by the model.
            - "testbench": The testbench information from the dataset.
            - "module_name": The module name corresponding to the task.

    Usage example:
        >>> df_inputs = generate_systemc("data.json", model=my_model, tokenizer=my_tokenizer)
        >>> print(df_inputs.head())

    Note:
        Within the scope of this function, a helper function 'collate_inputs' is defined, which can be used
        to collate a series of tokenized inputs by padding sequences to create batches compatible with the model.
    """

    # Load the dataset JSON
    dataset = os.path.join(dataset_dir, dataset)
    df = pd.read_json(dataset, orient="records")

    # Build the dataframe for the model input
    df_input = pd.DataFrame(
        columns=["task", "prompt", "input", "testbench", "module_name"]
    )

    # Generate embeddings and record the informations
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

    # Generate the responses and response batch by batch
    for i in tqdm(range(0, len(df_input), batch_size), desc="Generating Responses"):
        batch = df_input[i : i + batch_size]
        batch_inputs = collate_inputs(batch["input"])
        with torch.no_grad():
            output = model.generate(**batch_inputs, max_new_tokens=max_new_tokens)
        for j in range(len(batch)):
            decoded_output = tokenizer.decode(output[j], skip_special_tokens=False)
            df_input.loc[i + j, "respond"] = response_extractor(decoded_output)
            df_input.loc[i + j, "output_code"] = (
                code_extractor(
                    response_extractor(decoded_output),
                    df_input.loc[i + j, "module_name"],
                )
                + "\n\n"
                + df_input.loc[i + j, "testbench"]
            )

    return df_input


def response_extractor(response: str) -> str:
    pattern = r"<\|im_start\|>assistant(.*?)<\|im_end\|>"
    if re.search(pattern, response, re.DOTALL):
        matches = re.findall(pattern, response, re.DOTALL)
        return matches[0]
    else:
        print(response)
        warnings.warn("Warning: No response found in the output.")
        return ""  # No response found


def code_extractor(response: str, true_module_name) -> str:
    pattern_1 = r"```cpp(.*?)```"
    pattern_2 = r"```(.*?)```"

    def pattern_extractor(pattern, response):
        matches = re.findall(pattern, response, re.DOTALL)
        matches = [m for m in matches if "#include <systemc.h>" in m]

        if not matches:
            warnings.warn("Warning: No SystemC code snippet found in the response.")
            print(response)
            return ""
        # Extract the match that is longest in length
        code_snippet = max(matches, key=len)
        modules = re.split(r"(SC_MODULE|int sc_main)", code_snippet)
        if len(modules) <= 1:
            warnings.warn(
                "Warning: The extracted code snippet does not contain the expected SystemC tokens."
            )
            print(code_snippet)
            return ""

        header = modules[0]
        modules = modules[1:]
        try:
            modules = [modules[i] + modules[i + 1] for i in range(0, len(modules), 2)]
        except IndexError as e:
            warnings.warn(
                "Warning: Module splitting did not occur as expected. Please check the formatting of the code snippet."
            )
            print(modules)
            return ""

        # Remove testbench and main, if present
        modules = [
            module for module in modules if "sc_module(testbench)" not in module.lower()
        ]
        modules = [module for module in modules if "sc_main" not in module.lower()]

        if not modules:
            warnings.warn(
                "Warning: No modules found in the code snippet after filtering."
            )
            print(code_snippet)
            return ""

        # Change the name of the first module
        module_name_pattern = r"SC_MODULE\((.*?)\)"
        modules_name = re.findall(module_name_pattern, modules[0])
        if not modules_name:
            warnings.warn(
                "Warning: Could not extract module name from the first module."
            )
            print(modules[0])
            return ""

        modules_name = modules_name[0]
        code_snippet = header + "\n" + "\n".join(modules)
        code_snippet = code_snippet.replace(modules_name, true_module_name)
        return code_snippet

    if re.search(pattern_1, response, re.DOTALL):
        return pattern_extractor(pattern_1, response)
    else:
        if re.search(pattern_2, response, re.DOTALL):
            return pattern_extractor(pattern_2, response)
        else:
            warnings.warn("Warning: No code snippet found in the response.")
            print(response)
            return ""  # No code snippet found


def output_dir_generator(
    df: pd.DataFrame, dataset: str, output_data_dir: str = "output"
):
    data_dir = os.path.join(dataset_dir, dataset)
    for i, row in tqdm(df.iterrows(), desc="Writing output files"):
        filename = row["task"]
        output_dir = os.path.join(data_dir, output_data_dir, filename)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        with open(
            os.path.join(data_dir, output_data_dir, filename, "main.cpp"), "w"
        ) as f:
            f.write(row["output_code"])
        with open(
            os.path.join(data_dir, output_data_dir, filename, "response.txt"), "w"
        ) as f:
            f.write(row["respond"])


if __name__ == "__main__":

    model_name = "Qwen/Qwen2.5-Coder-32B-Instruct"  # Qwen/Qwen2.5-Coder-7B-Instruct, Qwen/Qwen2.5-Coder-14B-Instruct, Qwen/Qwen2.5-Coder-32B-Instruct
    dataset = "geek"
    data = "zero_shot.json"
    dtype = "bfloat16"  # int8, float16, bfloat16, None
    epoach = 1

    if not model_name == "Qwen/Qwen2.5-Coder-32B-Instruct" and dtype == "float16":
        warnings.warn(
            "Warning: The model is not Qwen/Qwen2.5-Coder-32B-Instruct, but the dtype is set to float16. This may cause issues."
        )

    tokenizer = AutoTokenizer.from_pretrained(model_name)
    tokenizer.padding_side = "left"

    if dtype == "float16":
        model = AutoModelForCausalLM.from_pretrained(
            model_name,
            torch_dtype=torch.float16,
            device_map="auto",
        )
    elif dtype == "bfloat16":
        model = AutoModelForCausalLM.from_pretrained(
            model_name,
            torch_dtype=torch.bfloat16,
            device_map="auto",
        )
    elif dtype == "int8":
        model = AutoModelForCausalLM.from_pretrained(
            model_name,
            load_in_8bit=True,
            device_map="auto",
        )
    elif dtype == None:
        model = AutoModelForCausalLM.from_pretrained(
            model_name,
            device_map="auto",
        )

    for i in range(1, epoach + 1):
        print(f"Epoch {i} / {epoach}")
        df_output = generate_systemc(
            os.path.join(dataset, data), model=model, tokenizer=tokenizer, batch_size=16
        )
        output_dir_generator(
            df_output, dataset, output_data_dir=f"{model_name}/output_{i}"
        )
        output_dir = os.path.join(dataset_dir, dataset, model_name)
        os.system(
            f"python3 systemc_generation/check.py --file_dir {output_dir}/output_{i} --log_dir {output_dir}"
        )
