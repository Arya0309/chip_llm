# utils.py
from __future__ import annotations

from typing import Any, Dict
import torch
from vllm import LLM, SamplingParams
from transformers import AutoTokenizer

DEFAULT_MODEL = "Qwen/Qwen2.5-Coder-7B-Instruct"


class VLLMGenerator:
    _ENGINE_CACHE: Dict[str, LLM] = {}
    _TOKENIZER_CACHE: Dict[str, AutoTokenizer] = {}

    def __init__(
        self,
        model_name: str = DEFAULT_MODEL,
        *,
        tp_size: int | None = None,
        dtype: str | None = "auto",
        **engine_kwargs: Any,
    ):
        tp_size = tp_size or torch.cuda.device_count() or 1
        print(f"Using {tp_size} GPU(s) via tensor-parallelism")

        # ----------------  Engine  ----------------
        if model_name not in self._ENGINE_CACHE:
            self._ENGINE_CACHE[model_name] = LLM(
                model=model_name,
                tensor_parallel_size=tp_size,
                trust_remote_code=True,
                dtype=dtype,
                **engine_kwargs,
            )
        self.llm: LLM = self._ENGINE_CACHE[model_name]

        # ---------------- Tokenizer ---------------
        if model_name not in self._TOKENIZER_CACHE:
            self._TOKENIZER_CACHE[model_name] = AutoTokenizer.from_pretrained(
                model_name,
                trust_remote_code=True,
            )
        self.tokenizer: AutoTokenizer = self._TOKENIZER_CACHE[model_name]

    def apply_chat_template(
        self,
        messages,
        *,
        tokenize: bool = False,
        add_generation_prompt: bool = True,
    ):
        return self.tokenizer.apply_chat_template(
            messages,
            tokenize=tokenize,
            add_generation_prompt=add_generation_prompt,
        )

    def generate(
        self,
        prompt: str,
        *,
        max_new_tokens: int = 4096,
        temperature: float = 0.7,
        top_p: float = 0.8,
        top_k: int = 20,
        repetition_penalty: float = 1.05,
    ) -> str:
        sampling_params = SamplingParams(
            max_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
            top_k=top_k,
            repetition_penalty=repetition_penalty,
        )
        outputs = self.llm.generate([prompt], sampling_params)
        return outputs[0].outputs[0].text

    __call__ = generate  # 允許像函式一樣直接呼叫


def get_cmake_list() -> str:
    cmake_list = """cmake_minimum_required(VERSION 3.8)

# Define the project name of these source code
project (test-dut)

# Define the C++ standard -std=c++17
# This is required to match the systemc installed with Ubuntu
set (CMAKE_CXX_STANDARD 17)

# Add optimization & warning flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")

# Additional line for A6000 Machine
set(SYSTEMC_HOME /usr/local/systemc-2.3.3)
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64)

# Set clock period = 10 ns
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D CLOCK_PERIOD=10.0")

# Set NATIVE_SYSTEMC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D NATIVE_SYSTEMC")

# Define the executable name and its source code
file(GLOB SRC "*.cpp")
add_executable(test-dut ${SRC})

# Define the used libraries of the executable, Equal to -l flags of g++
target_link_libraries(test-dut systemc)"""
    return cmake_list


if __name__ == "__main__":
    model_name = "Qwen/Qwen2.5-Coder-7B-Instruct"
    generator = VLLMGenerator(model_name)

    prompt = "Write a Python function to calculate the factorial of a number."
    print(generator(prompt))
