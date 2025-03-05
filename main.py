from cpp_to_text import accelerated_cpp_to_text
from text_to_systemc import text_to_systemc
import os
import json
import pandas
import re
from tqdm import tqdm
from transformers import AutoTokenizer, AutoModelForCausalLM
import torch

output_batch = []

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

text = accelerated_cpp_to_text(df_code[0], tokenizer=tokenizer, model=model)
systemc_code, title, raw_text = text_to_systemc(text, tokenizer=tokenizer, model=model)
print(systemc_code)

with open(os.path.join(dataset, "output_batch.json"), "w") as f:
    json.dump(output_batch, f, indent=4)
