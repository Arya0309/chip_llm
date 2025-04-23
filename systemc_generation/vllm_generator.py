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

cache_dir: str = "/workspace/models"  # Directory to save the model and reuse it


@dataclass
class ModelConfig:
    """Configuration class for the model and dataset.

    Args:
        model_name  (str):  Name of the model to be used.
        input_dir   (str):  Path to the pre-processed dataset.

        params      (SamplingParams):   Parameters for sampling.
        batch_size  (int):              Batch size for the dataloader. <-- Design a new dataclass to save the parameters

        current_dir     (str): Current working directory.
        root_name       (str): Name of the root directory for saving outputs.
        obj_dataset     (str): Name of the objective dataset.
        output_dir_name (str): Name of the output directory.

    Methods:
        root_dir (str):  Returns the root directory for saving outputs.
        output_dir (str): Returns the output directory for saving outputs.

    Example:
        config = ModelConfig()
        print(config.model_name)  # Output: "Qwen/Qwen2.5-Coder-1.5B-Instruct"
        print(config.input_dir)   # Output: "./dataset/geek/one_shot_sp.json"
    """

    # ---------- Model related ----------
    model_name: str = "Qwen/Qwen2.5-Coder-7B-Instruct"
    input_dir: str = (
        "./systemc_generation/dataset/geek/one_shot_sp.json"  # Pre-processed dataset
    )

    # ---------- Model parameters ----------
    # Used for vllm
    params: SamplingParams = field(
        default_factory=lambda: SamplingParams(
            max_tokens=4096,
        )
    )
    batch_size: int = 2

    # ---------- Save related ----------
    current_dir: str = os.path.dirname(os.path.realpath(__file__))  # Working directory
    root_name: str = "dataset"  # The name of root directory for saving the outputs
    obj_dataset: str = "geek"  # The name of the objective dataset
    output_dir_name: str = "output"  # The name of the output directory

    @property
    # Root directory of saving the generated outputs
    def root_dir(self) -> str:
        return os.path.join(self.current_dir, self.root_name)

    @property
    # Output directory of saving the generated outputs
    def output_dir(self) -> str:
        return os.path.join(
            self.root_dir, self.obj_dataset, self.model_name, self.output_dir_name
        )


def load_model(config: ModelConfig = ModelConfig()):
    """Load the model and tokenizer from the cache directory.
    If the model is not found in the cache directory, it will be downloaded.

    Args:
        config  (ModelConfig):  Only use the model_name and cache_dir from the config.

    Returns:
        llm (LLM): The loaded LLM model.
        tokenizer (AutoTokenizer): The loaded tokenizer.

    Example:
        config = ModelConfig()
        llm, tokenizer = load_model(config)
    """

    def download_model(model_name, model_dir):
        AutoTokenizer.from_pretrained(model_name).save_pretrained(model_dir)
        AutoModelForCausalLM.from_pretrained(model_name).save_pretrained(model_dir)

    model_dir = os.path.join(cache_dir, config.model_name)

    if not os.path.exists(model_dir):
        download_model(config.model_name, model_dir)

    tokenizer = AutoTokenizer.from_pretrained(model_dir)
    llm = LLM(model=model_dir)

    return llm, tokenizer


class SystemCGenerator:
    """SystemC code generator using VLLM.

    Args:
        config  (ModelConfig):  Configuration object for the model and dataset.

    Methods:
        generate_systemc(model, tokenizer): Generates SystemC code using the model and tokenizer.
        save_systemc(df, config): Saves the generated SystemC code to files.
        check_results(): Checks the results of the generated SystemC code.

    Example:
        config = ModelConfig()
        sc_generator = SystemCGenerator(config)
        llm, tokenizer = load_model(config)
        outputs = sc_generator.generate_systemc(llm, tokenizer)
        sc_generator.save_systemc(outputs, config)
        sc_generator.check_results()
    """

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
        """Generates SystemC code using the model and tokenizer.

        Args:
            model (LLM): The loaded LLM model.
            tokenizer (AutoTokenizer): The loaded tokenizer.

        Returns:
            outputs (pd.DataFrame): A DataFrame containing the generated SystemC code and other information.

        Example:
            config = ModelConfig()
            llm, tokenizer = load_model(config)
            sc_generator = SystemCGenerator(config)
            outputs = sc_generator.generate_systemc(llm, tokenizer)
        """
        dataset = SystemCDataset(self.config.input_dir, tokenizer)  # Load the dataset
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

    def save_systemc(self, df: pd.DataFrame):
        """Saves the generated SystemC code to files.

        Args:
            df (pd.DataFrame): A DataFrame containing the generated SystemC code and other information.

        Example:
            config = ModelConfig()
            sc_generator = SystemCGenerator(config)
            outputs = sc_generator.generate_systemc(llm, tokenizer)
            sc_generator.save_systemc(outputs)
        """
        for i, row in tqdm(df.iterrows(), desc="Writing output files"):
            filename = row["module_name"]
            file_dir = os.path.join(config.output_dir, filename)
            if not os.path.exists(file_dir):
                os.makedirs(file_dir)

            with open(os.path.join(file_dir, "main.cpp"), "w") as f:
                f.write(row["output_code"])
            with open(os.path.join(file_dir, "response.txt"), "w") as f:
                f.write(row["response"])

    def check_results(self):
        subprocess.run(
            [
                "python3",
                "systemc_generation/check.py",
                "--file_dir",
                config.output_dir,
                "--log_dir",
                f"{config.output_dir}/..",
            ]
        )


if __name__ == "__main__":
    epoches = 200
    config = ModelConfig()
    llm, tokenizer = load_model(config)

    sc_generator = SystemCGenerator(config)
    for i in range(epoches):
        print(f"Epoch {i + 1}/{epoches}")
        sc_generator.config.output_dir_name = f"output_{i}"

        outputs = sc_generator.generate_systemc(llm, tokenizer)
        sc_generator.save_systemc(outputs)
        sc_generator.check_results()
