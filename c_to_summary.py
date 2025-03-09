import os
import torch
import re
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM


def construct_prompt(code: str) -> str:

    prompt = """
Given the following C/C++ code,
{CODE}
Your task is to perform a detailed chain-of-thought analysis of the code. Please follow these instructions:

1. Decompose the code line-by-line or block-by-block.
2. For each part, explain its purpose and functionality in natural language.
3. Include explanations for:
   - Class and struct definitions: Describe what data is stored and why.
   - Function or method definitions: Explain the logic, inputs, and outputs.
   - Control flow statements (if, loops, etc.): Describe what conditions or iterations are happening.
   - Variable declarations and assignments: Explain their roles in the overall logic.
   - Any comments present in the code, and how they relate to the code functionality.
4. Provide a clear summary at the end that captures the overall design and logic of the code.
5. Ensure that your explanation is detailed enough for someone to understand how the code works even if they are not familiar with it.

Please begin your analysis with a statement like “Step-by-step Explanation:” and then provide your chain-of-thought reasoning. Use clear, concise, and descriptive language.
"""

    prompt = prompt.format(CODE=code)

    messages = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are a coding expert with deep knowledge of C++ and software design.",
        },
        {"role": "user", "content": prompt},
    ]

    return messages


def generate_summary(code: str, model=None, tokenizer=None, max_new_tokens=4096) -> str:
    prompt = construct_prompt(code)
    input = tokenizer.apply_chat_template(
        prompt, tokenize=False, add_generation_prompt=True
    )
    input = tokenizer(input, return_tensors="pt", padding=True, truncation=True).to(
        next(model.parameters()).device
    )
    print("Generating summary...")
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
    code_path = os.path.join(dataset, "AddTwoNumbers.cpp")

    with open(code_path, "r") as f:
        code = f.read()

    summary = generate_summary(code, model=model, tokenizer=tokenizer)
    print(summary)

    with open(os.path.join(dataset, "summary.txt"), "w") as f:
        f.write(summary)
