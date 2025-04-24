import os
import yaml
import argparse
from dataclasses import dataclass, field
from vllm import SamplingParams


@dataclass
class ModelConfig:
    model_name: str
    input_dir: str  # Pre-processed dataset

    epoches: int
    batch_size: int
    tensor_parallel_size: int
    gpu_memory_utilization: float

    root_name: str = "dataset"  # The name of root directory for saving the outputs
    obj_dataset: str = "geek"  # The name of the objective dataset
    output_dir_name: str = "outputs"  # The name of the output directory

    params: SamplingParams = field(default_factory=lambda: SamplingParams())

    work_dir: str = field(
        default_factory=lambda: os.path.dirname(os.path.realpath(__file__))
    )

    @property
    def root_dir(self) -> str:
        return os.path.join(self.work_dir, self.root_name)

    @property
    def output_dir(self) -> str:
        return os.path.join(
            self.root_dir, self.obj_dataset, self.model_name, self.output_dir_name
        )


def load_config(path: str) -> dict:
    with open(path, "r") as f:
        return yaml.safe_load(f)


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--config_path", type=str, required=True)
    parser.add_argument(
        "--generation_config",
        type=str,
        default="./generation_config/{model_name}.yaml",
    )

    return parser.parse_args()


def build_config():
    args = parse_args()
    config = load_config(args.config_path)

    # Load the SamplingParams from the generation config
    generation_config_path = args.generation_config.format(
        model_name=config["model_name"]
    )
    generation_config = load_config(generation_config_path)
    params = SamplingParams(**generation_config["params"])

    config["params"] = params
    return ModelConfig(**config)


if __name__ == "__main__":
    config = build_config()
    print(config)
    print(config.output_dir)
    print(config.root_dir)
    print(config.params)
