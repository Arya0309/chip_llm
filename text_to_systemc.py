import json
import os
import torch
import pandas
import re
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM


def construct_prompt(text: str) -> str:
    prompt = """Using the input data provided below, generate a clean, well-formatted SystemC module that meets the following requirements:

### Requirements:

1. **Module Declaration:**  
   - Derive a legal SystemC module name from the "title" field by removing or replacing spaces and any special characters to ensure compliance with C++ identifier rules.  
   - Declare the module using the `SC_MODULE` macro.

2. **Port and Signal Declarations:**  
   - Process the "parameters" field, which is a array of objects.  
   - For each parameter (which includes a "name" and "description"), if it maps directly to a SystemC port, declare it appropriately.  
   - If a parameter does not directly correspond to a port, include an inline comment explaining your assumptions.

3. **Documentation:**  
   - Insert the complete "description" field as a block comment at the top of the module.

4. **Functionality Translation:**  
   - Translate the functionality from the "code" field into equivalent SystemC constructs where possible.  
   - If a direct translation is not feasible, embed the original C++ code as a comment within the module.

5. **Output Requirements:**  
   - The output must include **only** the valid SystemC code.  
   - Ensure that the code is clear, readable, and contains no extraneous commentary outside of the code.
   - **Crucially, if the original C++ code does not include the necessary header directives, you must add them.** Include all required SystemC headers (e.g., `#include <systemc.h>`) and any additional includes needed for a complete, self-contained module.


### Input Data:

"title": "{title}",  
"parameters": {parameters},  
"description": "{description}",  
"code": "{code}"

1. "title": A short, descriptive title of the function or class (e.g., "Two Sum" or "Binary Search").
2. "parameters": A list of parameter objects. Each object should have:
   - "name": the name of the parameter,
   - "description": a short explanation of what the parameter is or does.
3. "description": A detailed explanation of the purpose and effect of the code, including key functionalities, the algorithmic approach, and any noteworthy implementation details.
4. "code": The original C++ code that implements the described functionality.

Generate the corresponding SystemC module based on the above instructions.
Begin.
"""
    prompt = prompt.format(
        title=text["title"],
        parameters=text["parameters"],
        description=text["description"],
        code=text["code"],
    )
    messages = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are a helpful code conversion assistant specialized in transforming C++ code descriptions into valid SystemC modules.",
        },
        {"role": "user", "content": prompt},
    ]

    return messages


def text_to_systemc(text: str, max_new_tokens=1024, tokenizer=None, model=None):
    messages = construct_prompt(text)

    print("Start SystemC code generation...")

    inputs = tokenizer.apply_chat_template(
        messages, tokenize=False, add_generation_prompt=True
    )
    inputs = tokenizer(inputs, return_tensors="pt", padding=True, truncation=True).to(
        next(model.parameters()).device
    )

    with torch.no_grad():
        outputs = model.generate(
            **inputs,
            max_new_tokens=max_new_tokens,
        )
    raw_text = tokenizer.decode(outputs[0], skip_special_tokens=False)

    if "<|im_start|>assistant" in raw_text:
        response = raw_text.split("<|im_start|>assistant")[1]
        response = response.split("<|im_end|>")[0]
    else:
        raise Exception("Missing assistant token.")
    systemc_code = raw_text.strip()

    systemc_code = get_code(systemc_code)
    systemc_code = check_sc_main(systemc_code)

    return systemc_code, text["title"], raw_text


def get_code(response: str) -> str:
    pattern = r"```(?:\w+)?\n([\s\S]*?)```"
    match = re.search(pattern, response, re.DOTALL)
    if match:
        return match.group(1).strip()
    return ""


def check_sc_main(systemc_code: str) -> str:
    if "sc_main" not in systemc_code:
        systemc_code = f"{systemc_code}\n\nint sc_main(int argc, char* argv[]) {{\n    return 0;\n}}"
    return systemc_code


def multiple_text_to_systemc(codes, tokenizer=None, model=None):

    for code in tqdm(codes):
        systemc_code, title, raw_text = text_to_systemc(
            code, tokenizer=tokenizer, model=model
        )

        os.makedirs(os.path.join(dataset, f"systemc_code/{title}"), exist_ok=True)

        with open(os.path.join(dataset, f"systemc_code/{title}/main.cpp"), "w") as file:
            file.write(systemc_code)
        with open(os.path.join(dataset, f"systemc_code/{title}/raw.txt"), "w") as file:
            file.write(raw_text)


if __name__ == "__main__":

    tokenizer = AutoTokenizer.from_pretrained("Qwen/Qwen2.5-Coder-32B-Instruct")
    model = AutoModelForCausalLM.from_pretrained(
        "Qwen/Qwen2.5-Coder-32B-Instruct",
        torch_dtype=torch.float16,
        device_map="auto",
    )

    current_dir = os.path.dirname(os.path.realpath(__file__))
    dataset = os.path.join(current_dir, "dataset")

    with open(os.path.join(dataset, "output_500.json"), "r") as file:
        cpp_code = json.load(file)

    multiple_text_to_systemc(cpp_code[:100], tokenizer=tokenizer, model=model)
