import os
import re
import warnings
import subprocess
import pandas as pd
from tqdm import tqdm
from dataclasses import dataclass, field
from vllm import LLM, SamplingParams
from transformers import AutoModelForCausalLM, AutoTokenizer
from SystemCDataset import SystemCDataset, collate_fn
from torch.utils.data import DataLoader


@dataclass
class ModelConfig:
    # ---------- Model related ----------
    model_name: str = "Qwen/Qwen2.5-Coder-1.5B-Instruct"
    cache_dir: str = "/workspace/models"
    input_dir: str = (
        "/home/S113062615/chip_llm/systemc_generation/dataset/geek/one_shot_sp.json"
    )
    params: SamplingParams = field(
        default_factory=lambda: SamplingParams(max_tokens=4096)
    )
    batch_size: int = 16

    # ---------- Save related ----------
    current_dir: str = os.path.dirname(os.path.realpath(__file__))
    save_name: str = "dataset"
    obj_dataset: str = "geek"
    output_dir_name: str = "output"  # <-- 改這個會影響 output_dir

    @property
    def save_dir(self) -> str:
        return os.path.join(self.current_dir, self.save_name)

    @property
    def output_dir(self) -> str:
        return os.path.join(
            self.save_dir, self.obj_dataset, self.model_name, self.output_dir_name
        )


def load_model(config: ModelConfig = ModelConfig()):

    def download_model():
        AutoTokenizer.from_pretrained(config.model_name).save_pretrained(model_dir)
        AutoModelForCausalLM.from_pretrained(config.model_name).save_pretrained(
            model_dir
        )

    model_dir = os.path.join(config.cache_dir, config.model_name)

    if not os.path.exists(model_dir):
        download_model(config.model_name, config.cache_dir)

    tokenizer = AutoTokenizer.from_pretrained(model_dir)
    llm = LLM(model=model_dir)

    return llm, tokenizer


class SystemCGenerator:
    def __init__(self, config: ModelConfig = ModelConfig()):
        self.config = config

    def _generate(self, model, dataloader, params):
        results = []

        for batch in dataloader:
            inputs = batch["input"]
            task_ids = batch["task_id"]
            module_names = batch["module_name"]
            testbenches = batch["testbench"]

            outputs = model.generate(inputs, params)

            for task_id, testbench, module_name, result in zip(
                task_ids, testbenches, module_names, outputs
            ):
                response = result.outputs[0].text
                results.append(
                    {
                        "task_id": task_id,
                        "testbench": testbench,
                        "module_name": module_name,
                        "response": response,
                    }
                )

        return pd.DataFrame(results)

    def generate_systemc(self, model, tokenizer):
        dataset = SystemCDataset(self.config.input_dir, tokenizer)
        dataloader = DataLoader(
            dataset,
            batch_size=self.config.batch_size,
            shuffle=False,
            collate_fn=collate_fn,
        )

        outputs = self._generate(model, dataloader, self.config.params)
        # Extract the code from the generated text
        outputs["output_code"] = outputs.apply(
            lambda row: self._code_extractor(row["response"], row["module_name"])
            + "\n\n"
            + row["testbench"],
            axis=1,
        )
        return outputs

    def _code_extractor(self, response: str, true_module_name) -> str:
        pattern_1 = r"```cpp(.*?)```"
        pattern_2 = r"```(.*?)```"

        def pattern_extractor(pattern, response):
            matches = re.findall(pattern, response, re.DOTALL)
            matches = [m for m in matches if "#include <systemc.h>" in m]

            if not matches:
                warnings.warn("Warning: No SystemC code snippet found in the response.")
                # print(response)
                return ""
            # Extract the match that is longest in length
            code_snippet = max(matches, key=len)
            modules = re.split(r"(SC_MODULE|int sc_main)", code_snippet)
            if len(modules) <= 1:
                warnings.warn(
                    "Warning: The extracted code snippet does not contain the expected SystemC tokens."
                )
                # print(code_snippet)
                return ""

            header = modules[0]
            modules = modules[1:]
            try:
                modules = [
                    modules[i] + modules[i + 1] for i in range(0, len(modules), 2)
                ]
            except IndexError as e:
                warnings.warn(
                    "Warning: Module splitting did not occur as expected. Please check the formatting of the code snippet."
                )
                # print(modules)
                return ""

            # Remove testbench and main, if present
            modules = [
                module
                for module in modules
                if "sc_module(testbench)" not in module.lower()
            ]
            modules = [module for module in modules if "sc_main" not in module.lower()]

            if not modules:
                warnings.warn(
                    "Warning: No modules found in the code snippet after filtering."
                )
                # print(code_snippet)
                return ""

            # Change the name of the first module
            module_name_pattern = r"SC_MODULE\((.*?)\)"
            modules_name = re.findall(module_name_pattern, modules[0])
            if not modules_name:
                warnings.warn(
                    "Warning: Could not extract module name from the first module."
                )
                # print(modules[0])
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
                # print(response)
                return ""  # No code snippet found


def output_dir_generator(df: pd.DataFrame, config: ModelConfig = ModelConfig()):
    for i, row in tqdm(df.iterrows(), desc="Writing output files"):
        filename = row["module_name"]
        file_dir = os.path.join(config.output_dir, filename)
        if not os.path.exists(file_dir):
            os.makedirs(file_dir)

        with open(os.path.join(file_dir, "main.cpp"), "w") as f:
            f.write(row["output_code"])
        with open(os.path.join(file_dir, "response.txt"), "w") as f:
            f.write(row["response"])


def check_results(file_dir):
    subprocess.run(
        [
            "python3",
            "systemc_generation/check.py",
            "--file_dir",
            file_dir,
            "--log_dir",
            f"{file_dir}/..",
        ]
    )


if __name__ == "__main__":
    epoches = 2
    config = ModelConfig()
    llm, tokenizer = load_model(config)

    for i in range(epoches):
        print(f"Epoch {i + 1}/{epoches}")
        config.output_dir_name = f"output_{i}"

        sc_generator = SystemCGenerator(config)
        outputs = sc_generator.generate_systemc(llm, tokenizer)
        output_dir_generator(outputs, config)
        check_results(config.output_dir)
