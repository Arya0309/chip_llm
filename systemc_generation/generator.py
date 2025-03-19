import os
import torch
import re
import examples
from tqdm import tqdm
import time
from transformers import AutoTokenizer, AutoModelForCausalLM


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

```
{FEW_SHOT_EXAMPLE_1}
```

Ensure that the SystemC code you provide faithfully captures the logic of the original C++/C source.
Make the explanation thorough and accessible.
"""

    prompt = prompt.format(CODE=code, FEW_SHOT_EXAMPLE_1=examples.structure_examples_1)

    messages = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are a coding expert in C++/C code analysis and SystemC.",
        },
        {"role": "user", "content": prompt},
    ]

    return messages


def generate_systemc(code: str, model=None, tokenizer=None, max_new_tokens=4096) -> str:
    prompt = construct_prompt(code)
    input = tokenizer.apply_chat_template(
        prompt, tokenize=False, add_generation_prompt=True
    )
    input = tokenizer(input, return_tensors="pt", padding=True, truncation=True).to(
        next(model.parameters()).device
    )
    print("Processing...")
    with torch.no_grad():
        output = model.generate(**input, max_new_tokens=max_new_tokens)
    summary = tokenizer.decode(output[0], skip_special_tokens=False)

    return response_extractor(summary)


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

    current_dir = os.path.dirname(os.path.realpath(__file__))
    dataset = os.path.join(current_dir, "dataset", "geek")
    # code_path = os.path.join(dataset, "bubble_sort", "bubble_sort.cpp")

    # with open(code_path, "r") as f:
    #     code = f.read()

    # output = generate_systemc(code, model=model, tokenizer=tokenizer)
    # print(output)

    # with open(os.path.join(dataset, "output.txt"), "w") as f:
    #     f.write(output)

    os.makedirs(os.path.join(dataset, "output"), exist_ok=True)

    start = time.time()

    for filename in tqdm(os.listdir(dataset)):
        try:
            code_path = os.path.join(dataset, filename)
            with open(code_path, "r") as f:
                code = f.read()

            output = generate_systemc(code, model=model, tokenizer=tokenizer)
            code = code_extractor(output)
            os.makedirs(os.path.join(dataset, "output", f"{filename}"), exist_ok=True)
            output_path = os.path.join(dataset, "output", f"{filename}")
            with open(os.path.join(output_path, "output.txt"), "w") as f:
                f.write(output)
            with open(os.path.join(output_path, "main.cpp"), "w") as f:
                f.write(code)
            print(f"Processed: {filename}")
        except Exception as e:
            print(f"Error processing: {filename}")

    end = time.time()
    print(f"Time taken: {end - start}")
