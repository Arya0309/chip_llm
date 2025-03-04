import json
import os
import torch
import pandas
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM

# Few-shot examples to guide the model format
FEW_SHOT_EXAMPLES_1 = (
    "Example 1: "
    '{"title": "Two Sum", '
    '"parameters": ['
    '{"name": "nums", "description": "Input array of integers"}, '
    '{"name": "target", "description": "Target sum value"}], '
    "\"description\": \"Find two distinct indices in the array 'nums' such that the values at those indices sum to 'target'. Return those indices.\"}"
)

FEW_SHOT_EXAMPLES_2 = (
    "Example 2: "
    '{"title": "Binary Search", '
    '"parameters": ['
    '{"name": "arr", "description": "Sorted array of integers"}, '
    '{"name": "target", "description": "The integer value to search for"}], '
    "\"description\": \"Performs binary search to determine if 'target' exists in 'arr'.\"}"
)


def auto_fix_json(text):
    text = text.strip()

    if text.startswith('"') and text.endswith('"'):
        try:
            text = json.loads(text)
        except json.JSONDecodeError:
            text = text[1:-1].strip()

    if text.startswith("```json"):
        text = text[len("```json") :].strip()
    if text.startswith("```"):
        text = text[len("```") :].strip()
    if text.endswith("```"):
        text = text[:-3].strip()

    if "End." in text:
        text = text.split("End.")[0].strip()

    def extract_first_json(s):
        start = s.find("{")
        if start == -1:
            return s
        count = 0
        for i in range(start, len(s)):
            if s[i] == "{":
                count += 1
            elif s[i] == "}":
                count -= 1
                if count == 0:
                    return s[start : i + 1]
        return s[start:]

    text = extract_first_json(text)
    return text


def cpp_to_text(code, max_attempts=3):
    attempt = 0
    model.eval()
    while attempt < max_attempts:
        attempt += 1
        prompt = f"""I will provide you with a snippet of C++ code, and I want you to analyze what the code does. Then, return a concise JSON-formatted description with three fields:

1. "title": A short, descriptive title of the function or class (e.g., "Two Sum" or "Binary Search").
2. "parameters": A list of parameter objects. Each object should have:
   - "name": the name of the parameter,
   - "description": a short explanation of what the parameter is or does.
3. "description": A detailed explanation of the purpose and effect of the code, including key functionalities, the algorithmic approach, and any noteworthy implementation details.


Below are examples of the desired JSON output:

{FEW_SHOT_EXAMPLES_1}

{FEW_SHOT_EXAMPLES_2}

Now, please analyze the following C++ code and return only valid JSON in the same format as the examples above, with no additional commentary or text.

**C++ Code:**
{code}

Begin.
"""
        messages = [
            {
                "role": "system",
                "content": "You are Qwen, created by Alibaba Cloud. You are a helpful code interpreter.",
            },
            {"role": "user", "content": prompt},
        ]

        inputs = tokenizer.apply_chat_template(messages, return_tensors="pt")
        inputs = {"input_ids": inputs}
        inputs["attention_mask"] = (
            inputs["input_ids"] != tokenizer.pad_token_id
        ).long()
        inputs = {
            key: value.to(next(model.parameters()).device)
            for key, value in inputs.items()
        }

        with torch.no_grad():
            outputs = model.generate(
                **inputs,
                max_new_tokens=512,
            )
        response = tokenizer.batch_decode(outputs, skip_special_tokens=True)[0]

        if "Begin." in response:
            response = response.split("Begin.", 1)[1]
        generated_text = response.strip()

        fixed_response = auto_fix_json(generated_text)

        try:
            parsed_json = json.loads(fixed_response)
            parsed_json["code"] = code

            if attempt == max_attempts:
                print("Missing one or more required keys")
                return parsed_json

            required_keys = ["title", "parameters", "description"]

            if not all(key in parsed_json for key in required_keys):
                continue

            return parsed_json
        except json.JSONDecodeError as e:
            if attempt == max_attempts:
                print(f"Failed to parse JSON after {max_attempts} attempts.")
                print(f"Error: {e}")
                print(f"Original code:\n{code}")
                return fixed_response.append({"code": code})


def multi_cpp_to_text(df_code: pandas.DataFrame):
    output_batch = []

    for code in tqdm(df_code):
        output = cpp_to_text(code)
        output_batch.append(output)

    return output_batch


if __name__ == "__main__":

    tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-32B-Instruct")
    model = AutoModelForCausalLM.from_pretrained(
        "Qwen/Qwen2.5-Coder-32B-Instruct",
        torch_dtype=torch.float16,
        device_map="auto",
    )

    current_dir = os.path.dirname(os.path.realpath(__file__))
    dataset = os.path.join(current_dir, "dataset")
    code_path = os.path.join(dataset, "database.csv")

    output_batch = []

    with open(code_path, "r") as f:
        df = pandas.read_csv(f)
        df_code = df["Answer"]

    output_batch = multi_cpp_to_text(df_code[:10])

    # cpp_to_text(df_code[0])

    with open(os.path.join(dataset, "output_batch.json"), "w") as f:
        json.dump(output_batch, f, indent=4)
