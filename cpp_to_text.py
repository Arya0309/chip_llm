import json
import os
import re
import torch
import pandas
import random
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

required_keys = ["title", "parameters", "description"]


import json


def extract_json(text) -> dict:
    text = text.strip()
    start = text.find("{")
    if start == -1:
        return {}
    count = 0
    for i in range(start, len(text)):
        if text[i] == "{":
            count += 1
        elif text[i] == "}":
            count -= 1
            if count == 0:
                json_str = text[start : i + 1]
                break
    else:
        json_str = text[start:]

    try:
        data = json.loads(json_str)
    except json.JSONDecodeError as e:
        raise ValueError(f"JSONDecodeError: {e}")

    if not isinstance(data, dict):
        raise ValueError("JSON must be a dictionary.")
    return data


def check_json(text):
    try:
        json.loads(text)

        if all(key in text for key in required_keys):
            return True
        else:
            print("Missing some required keys.")
            return False
    except json.JSONDecodeError as e:
        print("JSONDecodeError:", e)
        return False


def construct_prompt(code: str):
    prompt = f"""
I will provide you with a snippet of C++ code, and I want you to analyze what the code does. Then, return a concise JSON-formatted description with three fields:

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
{code}"""

    message = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are a helpful code interpreter.",
        },
        {
            "role": "user",
            "content": prompt,
        },
    ]
    return message


def cpp_to_text(
    code: str,
    attempt_size=1,
    max_new_tokens=1024,
    candidate=1,
    tokenizer=None,
    model=None,
) -> dict:
    accumulator = []

    trial = 0
    print("Start description generation...")
    while len(accumulator) < candidate:
        episodes = []
        for i in range(attempt_size):
            episodes.append(
                {"attempt": i + 1, "code": code, "complete": False, "result": None}
            )
        trial += 1

        batch_attempt = [episode for episode in episodes]
        batch_text = [construct_prompt(episode["code"]) for episode in batch_attempt]
        batch_text = [
            tokenizer.apply_chat_template(
                text, tokenize=False, add_generation_prompt=True
            )
            for text in batch_text
        ]
        inputs = tokenizer(
            batch_text, return_tensors="pt", padding=True, truncation=True
        ).to(next(model.parameters()).device)

        with torch.no_grad():
            outputs = model.generate(
                **inputs,
                max_new_tokens=max_new_tokens,
            )

        responses = tokenizer.batch_decode(outputs, skip_special_tokens=False)
        for episode, response in zip(batch_attempt, responses):
            print(f"Attempt: {episode['attempt']}/{attempt_size}")
            if "<|im_start|>assistant" in response:
                response = response.split("<|im_start|>assistant")[1]
                response = response.split("<|im_end|>")[0]
            else:
                print(f"Missing assistant token.")
                continue
            response = extract_json(response)

            response["code"] = episode["code"]
            episode["complete"] = True
            episode["result"] = response
            print("```")
            print(json.dumps(response, indent=4))
            print("```")
            accumulator.append(response)

    print("Description generation completed.")
    print("===================================================")

    def get_best_result(results: list) -> dict:
        return random.choice(results)

    return get_best_result(accumulator)


def batch_cpp_to_text(
    df_code: pandas.DataFrame,
    batch_size=32,
    max_attempts=20,
    max_new_tokens=1024,
    tokenizer=None,
    model=None,
):
    tasks = []
    for idx, code in enumerate(df_code):
        tasks.append(
            {
                "id": idx + 1,
                "code": code,
                "attempt": 0,
                "complete": False,
                "result": None,
            }
        )

    while not all(task["complete"] for task in tasks):
        incomplete_tasks = [task for task in tasks if not task["complete"]]
        if len(incomplete_tasks) < batch_size:
            batch_tasks = incomplete_tasks
        else:
            batch_tasks = incomplete_tasks[:batch_size]
        print(f"Number of incomplete tasks: {len(incomplete_tasks)}")

        batch_codes = [task["code"] for task in batch_tasks]
        batch_text = [construct_prompt(code) for code in batch_codes]
        batch_text = [
            tokenizer.apply_chat_template(
                text, tokenize=False, add_generation_prompt=True
            )
            for text in batch_text
        ]

        inputs = tokenizer(
            batch_text, return_tensors="pt", padding=True, truncation=True
        ).to(next(model.parameters()).device)

        with torch.no_grad():
            outputs = model.generate(
                **inputs,
                max_new_tokens=max_new_tokens,
            )

        responses = tokenizer.batch_decode(outputs, skip_special_tokens=False)

        for task, response in zip(batch_tasks, responses):
            task["attempt"] += 1
            if "<|im_start|>assistant" in response:
                response = response.split("<|im_start|>assistant")[1]
                response = response.split("<|im_end|>")[0]
            else:
                print(f"Task {task['id']} failed. #attempt: {task['attempt']}, error 1")
                continue

            response = extract_json(response)

            try:
                parsed_json = json.loads(response)
                parsed_json["code"] = task["code"]

                required_keys = ["title", "parameters", "description"]

                if not all(key in parsed_json for key in required_keys):
                    if task["attempt"] == max_attempts:
                        task["complete"] = True
                        task["result"] = response
                    print(
                        f"Task {task['id']} failed. #attempt: {task['attempt']}, error 2"
                    )
                    continue

                task["complete"] = True
                task["result"] = parsed_json
                print(f"Task {task['id']} completed.")
            except json.JSONDecodeError as e:
                if task["attempt"] == max_attempts:
                    task["complete"] = True
                    task["result"] = response
                print(f"Task {task['id']} failed. #attempt: {task['attempt']}, error 3")
                continue
    return [task["result"] for task in tasks]


if __name__ == "__main__":
    tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-32B-Instruct")
    tokenizer.padding_side = "left"
    model = AutoModelForCausalLM.from_pretrained(
        "Qwen/Qwen2.5-Coder-32B-Instruct",
        torch_dtype=torch.float16,
        device_map="auto",
    )

    current_dir = os.path.dirname(os.path.realpath(__file__))
    dataset = os.path.join(current_dir, "dataset")
    code_path = os.path.join(dataset, "database.csv")

    with open(code_path, "r") as f:
        df = pandas.read_csv(f)
        df_code = df["Answer"]

    import time

    start = time.time()
    output_batch = cpp_to_text(
        df_code[17],
        tokenizer=tokenizer,
        model=model,
    )
    end = time.time()
    print(f"Time elapsed: {end - start} seconds")

    with open(os.path.join(dataset, "output_batch.json"), "w") as f:
        json.dump(output_batch, f, indent=4)
