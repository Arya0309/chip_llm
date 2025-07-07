import os
import re
import warnings
import subprocess
import pandas as pd
from tqdm import tqdm
from vllm import LLM
from transformers import AutoModelForCausalLM, AutoTokenizer
from SystemCDataset import SystemCDataset, collate_fn
from torch.utils.data import DataLoader
from config import ModelConfig, build_config

cache_dir: str = "/workspace/models"  # Directory to save the model and reuse it
current_dir = os.path.dirname(os.path.realpath(__file__))
temp_format = os.path.join(current_dir, "__cache__", ".temp_{idx}")
temp_dir = None


def load_model(config: ModelConfig):
    """Load the model and tokenizer from the cache directory.
    If the model is not found in the cache directory, it will be downloaded.

    Args:
        config  (ModelConfig):  Configuration object for the model and dataset.

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
    llm = LLM(
        model=model_dir,
        tensor_parallel_size=config.tensor_parallel_size,
        gpu_memory_utilization=config.gpu_memory_utilization,
        dtype=config.dtype,
        task="generate",
    )

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

    def __init__(self, config: ModelConfig):
        self.config = config

    def _generate(self, model, dataloader) -> pd.DataFrame:
        results = []

        for batch in dataloader:
            inputs = batch["input"]
            task_ids = batch["task_id"]
            tasks = batch["task"]
            module_names = batch["module_name"]
            testbenches = batch["testbench"]

            # print(inputs)
            # exit()

            outputs = model.generate(inputs, self.config.params)

            for task_id, task, testbench, module_name, result in zip(
                task_ids, tasks, testbenches, module_names, outputs
            ):
                response = result.outputs[0].text
                results.append(
                    {
                        "task_id": task_id,
                        "task": task,
                        "testbench": testbench,
                        "module_name": module_name,
                        "response": response,
                    }
                )

        # Extract the code from the generated text
        results = pd.DataFrame(results)
        results["generated_code"] = results.apply(
            lambda row: self._code_extractor(row["response"], row["module_name"]),
            axis=1,
        )
        results["output_code"] = results.apply(
            lambda row: row["generated_code"] + "\n\n" + row["testbench"],
            axis=1,
        )

        return results

    def generate_systemc(self, model, tokenizer) -> pd.DataFrame:
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
        self.dataset = SystemCDataset(
            self.config.input_dir,
            tokenizer,
        )
        dataloader = DataLoader(
            self.dataset,
            batch_size=self.config.batch_size,
            shuffle=False,
            collate_fn=collate_fn,
        )

        if self.config.debug_round:
            print(f"Debug mode: at most {self.config.debug_round} rounds")
            return self._self_debug(model, dataloader)
        else:
            outputs = self._generate(model, dataloader)

        return outputs

    def _self_debug(self, model, dataloader):
        outputs = pd.DataFrame()
        for i in range(self.config.debug_round):
            print(f"Debug round {i + 1}/{self.config.debug_round}")
            results = self._generate(model, dataloader)

            done_results = pd.DataFrame(columns=results.columns)
            if i < self.config.debug_round - 1:
                self.save_systemc(results, temp=True)
                self.check_results(temp=True)

                # Map the responses to the dataset
                response_map = results.set_index("task")["generated_code"].to_dict()
                self.dataset.df["generated_code"] = (
                    self.dataset.df["task"]
                    .map(response_map)
                    .fillna(self.dataset.df["generated_code"])
                )

                # Check the results and update the dataset
                done, done_task = self.dataset.update_done(temp_dir, idx=i)
                done_results = results[results["task"].isin(done_task)]
                if done:
                    print("Early stopping: all tasks are done.")
                    break
                # Update the dataloader with the new dataset
                dataloader = DataLoader(
                    self.dataset,
                    batch_size=self.config.batch_size,
                    shuffle=False,
                    collate_fn=collate_fn,
                )

            outputs = pd.concat([outputs, done_results], ignore_index=True)

        outputs = pd.concat([outputs, results], ignore_index=True)
        return outputs

    def _code_extractor(self, response: str, true_module_name) -> str:
        pattern = r"```(.*?)```"

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
        try:
            header = "#include <systemc.h>" + header.split("#include <systemc.h>")[1]
        except IndexError as e:
            warnings.warn(
                "Warning: The header does not contain the expected SystemC include statement."
            )
            # print(header)
            return ""
        modules = modules[1:]
        try:
            modules = [modules[i] + modules[i + 1] for i in range(0, len(modules), 2)]
        except IndexError as e:
            warnings.warn(
                "Warning: Module splitting did not occur as expected. Please check the formatting of the code snippet."
            )
            # print(modules)
            return ""

        # Remove testbench and main, if present
        modules = [
            module for module in modules if "sc_module(testbench)" not in module.lower()
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

    def save_systemc(self, df: pd.DataFrame, temp=False):
        """Saves the generated SystemC code to files.

        Args:
            df (pd.DataFrame): A DataFrame containing the generated SystemC code and other information.

        Example:
            config = ModelConfig()
            sc_generator = SystemCGenerator(config)
            outputs = sc_generator.generate_systemc(llm, tokenizer)
            sc_generator.save_systemc(outputs)
        """
        if temp:
            for i, row in tqdm(df.iterrows(), desc="Writing output files"):
                filename = row["task"]
                file_dir = os.path.join(temp_dir, filename)
                if not os.path.exists(file_dir):
                    os.makedirs(file_dir)

                with open(os.path.join(file_dir, "main.cpp"), "w") as f:
                    f.write(row["output_code"])
                with open(os.path.join(file_dir, "response.txt"), "w") as f:
                    f.write(row["response"])
        else:
            for i, row in tqdm(df.iterrows(), desc="Writing output files"):
                filename = row["task"]
                file_dir = os.path.join(self.config.output_dir, filename)
                if not os.path.exists(file_dir):
                    os.makedirs(file_dir)

                with open(os.path.join(file_dir, "main.cpp"), "w") as f:
                    f.write(row["output_code"])
                with open(os.path.join(file_dir, "response.txt"), "w") as f:
                    f.write(row["response"])

    def check_results(self, temp=False, timeout=5):
        if temp:
            directory = temp_dir
            log_dir = directory
        else:
            directory = self.config.output_dir
            log_dir = os.path.join(directory, "..")

        # Call the check.py script to check the generated SystemC code
        subprocess.run(
            [
                "python3",
                "systemc_generation/check.py",
                "--file_dir",
                directory,
                "--log_dir",
                f"{log_dir}",
                "--timeout",
                str(timeout),
            ],
        )


if __name__ == "__main__":
    config = build_config()
    llm, tokenizer = load_model(config)

    sc_generator = SystemCGenerator(config)
    for i in range(config.epoches):
        print(f"Epoch {i + 1}/{config.epoches}")
        temp_dir = temp_format.format(idx=i)
        sc_generator.config.output_dir_name = f"output_{i}"

        outputs = sc_generator.generate_systemc(llm, tokenizer)
        sc_generator.save_systemc(outputs)
        sc_generator.check_results()
