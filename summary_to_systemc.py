import os
import torch
import re
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM


def construct_prompt(summary: str) -> list:

    prompt = f"""
Based on a detailed chain-of-thought natural language summary of some C++ code, your task is to design and generate a complete SystemC implementation that reproduces the same functionality. The complete code should include all necessary parts (including module definitions, testbench, main function, and any custom tracing functions if needed) so that it can be compiled and run as a standalone program.

Chain-of-Thought Summary:
{summary}

Please follow these instructions:

1. Identify the key components described in the summary (for example, custom data structures, processing modules, and control logic).
2. Map each component from the C++ code to an appropriate SystemC module:
   - Convert custom classes/structs to SystemC data structures.
   - Transform functions or methods into SC_METHOD or SC_THREAD processes within modules.
   - Define input and output ports (using sc_in, sc_out) for each module.
   - Specify the interconnections using sc_signal.
3. Design and include a complete testbench:
   - Create a testbench module that instantiates your SystemC module(s).
   - Drive the inputs with test data and check the outputs.
   - Include printouts or other verification methods to indicate whether the test passes or fails.
4. Include any necessary support functions (e.g., custom sc_trace functions) and the main() function that instantiates the testbench and starts the simulation.
5. The final output must be a complete, syntactically correct SystemC program that can be compiled and simulated without requiring additional code.

Begin your response with “SystemC Module Design:” and then provide the complete code.
"""

    messages = [
        {
            "role": "system",
            "content": "You are Qwen, created by Alibaba Cloud. You are now an expert in SystemC and hardware synthesis design.",
        },
        {"role": "user", "content": prompt},
    ]

    return messages


def generate_systemc_code(
    summary: str, model=None, tokenizer=None, max_new_tokens=4096
) -> str:
    prompt = construct_prompt(summary)
    input = tokenizer.apply_chat_template(
        prompt, tokenize=False, add_generation_prompt=True
    )
    input = tokenizer(input, return_tensors="pt", padding=True, truncation=True).to(
        next(model.parameters()).device
    )
    print("Generating SystemC code...")
    output = model.generate(**input, max_new_tokens=max_new_tokens)
    code = tokenizer.decode(output[0], skip_special_tokens=False)
    return response_extractor(code)


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
    summary_path = os.path.join(dataset, "summary.txt")

    with open(summary_path, "r") as f:
        summary = f.read()

    import time

    start = time.time()
    code = generate_systemc_code(summary, model=model, tokenizer=tokenizer)
    print(code)
    end = time.time()
    print(f"Time taken: {end-start} seconds")

    with open(os.path.join(dataset, "systemc_code.cpp"), "w") as f:
        f.write(code)
