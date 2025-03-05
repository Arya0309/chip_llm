import json
import os
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


def construct_prompt(code: str):
    prompt = f"""<|im_start|>system
You are Qwen, created by Alibaba Cloud. You are a helpful code interpreter.<|im_end|>
<|im_start|>user
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
{code}

Begin.
<|im_end|>
"""
    return prompt


def cpp_to_text(code: str, max_attempts=3) -> dict:
    attempt = 0
    model.eval()
    while attempt < max_attempts:
        attempt += 1
        prompt = construct_prompt(code)

        inputs = tokenizer(prompt, return_tensors="pt")
        print(tokenizer.decode(inputs["input_ids"][0]))
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


def batch_cpp_to_text(
    df_code: pandas.DataFrame, batch_size=16, max_attempts=20, max_tokens=4096
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
        batch_tasks = incomplete_tasks[:batch_size]
        print(f"Number of incomplete tasks: {len(incomplete_tasks)}")

        batch_codes = [task["code"] for task in batch_tasks]
        batch_text = [construct_prompt(code) for code in batch_codes]

        inputs = tokenizer(
            batch_text, return_tensors="pt", padding=True, truncation=True
        )
        inputs = {
            key: value.to(next(model.parameters()).device)
            for key, value in inputs.items()
        }

        with torch.no_grad():
            outputs = model.generate(
                **inputs,
                max_new_tokens=max_tokens,
            )

        responses = tokenizer.batch_decode(outputs, skip_special_tokens=True)

        for task, response in zip(batch_tasks, responses):
            task["attempt"] += 1
            if "Begin." in response:
                response = response.split("Begin.", 1)[1]
            generated_text = response.strip()
            fixed_response = auto_fix_json(generated_text)

            try:
                parsed_json = json.loads(fixed_response)
                parsed_json["code"] = task["code"]

                required_keys = ["title", "parameters", "description"]

                if not all(key in parsed_json for key in required_keys):
                    if task["attempt"] == max_attempts:
                        task["complete"] = True
                        task["result"] = fixed_response
                    print(f"Task {task['id']} failed. #attempt: {task['attempt']}")
                    continue

                task["complete"] = True
                task["result"] = parsed_json
                print(f"Task {task['id']} completed.")
            except json.JSONDecodeError as e:
                if task["attempt"] == max_attempts:
                    task["complete"] = True
                    task["result"] = fixed_response
                print(f"Task {task['id']} failed. #attempt: {task['attempt']}")
                continue
    return [task["result"] for task in tasks]


def get_best_result(results: list) -> dict:
    return random.choice(results)


def accelerated_cpp_to_text(
    code: str,
    attempt_size=5,
    max_new_tokens=4096,
    candidate=1,
    tokenizer=None,
    model=None,
) -> dict:
    accumulator = []

    trial = 0
    while len(accumulator) < candidate:
        episodes = []
        for i in range(attempt_size):
            episodes.append(
                {"attempt": i + 1, "code": code, "complete": False, "result": None}
            )
        trial += 1
        print(f"Starting trial {trial}. #attempt: {attempt_size}")

        batch_attempt = [episode for episode in episodes]
        batch_text = [construct_prompt(episode["code"]) for episode in batch_attempt]

        inputs = tokenizer(
            batch_text, return_tensors="pt", padding=True, truncation=True
        )
        inputs = {
            key: value.to(next(model.parameters()).device)
            for key, value in inputs.items()
        }

        with torch.no_grad():
            outputs = model.generate(
                **inputs,
                max_new_tokens=max_new_tokens,
            )

        responses = tokenizer.batch_decode(outputs, skip_special_tokens=True)

        for episode, response in zip(batch_attempt, responses):
            if "Begin." in response:
                response = response.split("Begin.", 1)[1]
            generated_text = response.strip()

            fixed_response = auto_fix_json(generated_text)

            try:
                parsed_json = json.loads(fixed_response)
                parsed_json["code"] = episode["code"]

                required_keys = ["title", "parameters", "description"]

                if not all(key in parsed_json for key in required_keys):
                    continue

                episode["complete"] = True
                episode["result"] = parsed_json
                print(f"Episode {episode['attempt']} completed.")
                accumulator.append(parsed_json)
            except json.JSONDecodeError as e:
                continue
        if len(accumulator) < candidate:
            print(
                f"Insufficient amount of candidates. #accumulated: {len(accumulator)}"
            )

    print("Candidates amount satisfied.")
    return get_best_result(accumulator)


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

    time_record = []
    for t in range(3):
        for i in [1, 5, 10, 20]:
            start_time = time.time()

            output_batch = accelerated_cpp_to_text(df_code[0], attempt_size=i)

            end_time = time.time()
            print(f"Time elapsed: {end_time - start_time} seconds")
            time_record.append({"attempt_size": i, "time": end_time - start_time})

    print(pandas.DataFrame(time_record))

    with open(os.path.join(dataset, "output_batch.json"), "w") as f:
        json.dump(output_batch, f, indent=4)
