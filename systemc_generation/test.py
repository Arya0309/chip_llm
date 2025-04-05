from transformers import AutoTokenizer, AutoModelForCausalLM
import torch
import pandas as pd

model = AutoModelForCausalLM.from_pretrained(
    "Qwen/Qwen2.5-Coder-32B-Instruct",
    torch_dtype=torch.float16,
    device_map="auto",
)
tokenizer = AutoTokenizer.from_pretrained(
    "Qwen/Qwen2.5-Coder-32B-Instruct",
    trust_remote_code=True,
)
tokenizer.padding_side = "left"

df_data = pd.read_json(
    "/home/S113062628/project/chip_llm/systemc_generation/data.json",
    orient="records",
)

prompt = df_data["prompt"].iloc[3]

input = tokenizer.apply_chat_template(
    prompt, tokenize=False, add_generation_prompt=True
)
input = tokenizer(
    input,
    padding=True,
    truncation=True,
    return_tensors="pt",
).to(next(model.parameters()).device)

output = model.generate(
    **input,
    max_new_tokens=4096,
)

output = tokenizer.decode(output[0], skip_special_tokens=False)

print(output)
