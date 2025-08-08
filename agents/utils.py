# utils.py
from __future__ import annotations
from pathlib import Path
import os
import random

from typing import Any, Dict
import torch

try:
    from vllm import LLM, SamplingParams
except ImportError:
    LLM = None
    SamplingParams = None
from transformers import AutoModelForCausalLM, AutoTokenizer

from transformers.distributed import DistributedConfig

DEFAULT_MODEL = os.getenv("LLM_MODEL", "openai/gpt-oss-20b")
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
INPUT_DATA_DIR = PROJECT_ROOT / "data_inputs"  # /home/.../chip_llm/data_inputs

import torch


class HFGenerator:
    def __init__(
        self,
        model_name: str,
        *,
        multi_gpu: bool | int = True,
        tp_plan: str | dict | None = "auto",
        enable_expert_parallel: int = 1,
        dtype: str | torch.dtype = "auto",
    ):
        self.tokenizer = AutoTokenizer.from_pretrained(model_name, padding_side="left")

        # ------------------ build device_map ------------------
        if multi_gpu:
            # Which GPUs?
            n_gpu_total = torch.cuda.device_count()
            if isinstance(multi_gpu, bool):
                n_gpu_use = n_gpu_total
            else:  # int
                n_gpu_use = min(int(multi_gpu), n_gpu_total)
            visible = list(range(n_gpu_use))

            # Tell HF how to shard the weights
            device_map: dict = {
                # tensor parallel
                "tp_plan": tp_plan,
                # expert parallel
                "distributed_config": DistributedConfig(
                    enable_expert_parallel=enable_expert_parallel
                ),
            }
            # You may pin layers manually here if desired, but "tp_plan":"auto"
            # already handles common architectures.
        else:
            device_map = {"device_map": "auto"}  # falls back to a single GPU

        self.model = AutoModelForCausalLM.from_pretrained(
            model_name,
            torch_dtype=dtype,
            attn_implementation="kernels-community/vllm-flash-attn3",  # identical to docs
            **device_map,
        )

        # A convenience handle to pick one device for prompt tensors later.
        self.main_device = next(self.model.parameters()).device

    # ------------------------------------------------------------------
    # Same public surface as before
    # ------------------------------------------------------------------
    def apply_chat_template(
        self,
        messages,
        *,
        tokenize: bool = False,
        add_generation_prompt: bool = True,
    ):
        if tokenize:  # used by .generate()
            return self.tokenizer.apply_chat_template(
                messages,
                tokenize=True,
                add_generation_prompt=add_generation_prompt,
                return_dict=True,
                return_tensors="pt",
            ).to(self.main_device)
        else:  # used by _chat_to_prompt()
            return self.tokenizer.apply_chat_template(
                messages,
                tokenize=False,
                add_generation_prompt=add_generation_prompt,
            )

    def generate(self, prompt: str, **generate_kwargs) -> str:
        # 1) chat → tokens
        inputs = self.apply_chat_template(
            [{"role": "user", "content": prompt}], tokenize=True
        )

        # 2) strip vLLM-only kwargs
        for bad in ("use_tqdm", "stream", "stop", "stop_token_ids", "echo"):
            generate_kwargs.pop(bad, None)

        # 3) HF generate()
        outputs = self.model.generate(
            **inputs,
            **generate_kwargs,  # temperature / top_p / max_new_tokens…
        )

        # 4) remove the prompt tokens
        return self.tokenizer.decode(
            outputs[0][inputs["input_ids"].shape[-1] :],
            skip_special_tokens=True,
        )


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
        temperature: float = 0.3,
        top_p: float = 0.8,
        top_k: int = 20,
        repetition_penalty: float = 1.05,
        use_tqdm: bool = True,
    ) -> str:
        sampling_params = SamplingParams(
            seed=random.randint(0, 2**31 - 1),
            max_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
            top_k=top_k,
            repetition_penalty=repetition_penalty,
        )
        outputs = self.llm.generate([prompt], sampling_params, use_tqdm=use_tqdm)
        return outputs[0].outputs[0].text

    __call__ = generate  # 允許像函式一樣直接呼叫

    def generate_batch(
        self,
        prompts: list[str],
        *,
        max_new_tokens: int = 4096,
        temperature: float = 0.3,
        top_p: float = 0.8,
        top_k: int = 20,
        repetition_penalty: float = 1.05,
        use_tqdm: bool = True,
    ) -> list[str]:
        """一次回傳 len(prompts) 條 completion；順序與輸入相同。"""
        params = SamplingParams(
            seed=random.randint(0, 2**31 - 1),
            max_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
            top_k=top_k,
            repetition_penalty=repetition_penalty,
        )
        outs = self.llm.generate(
            prompts, params, use_tqdm=use_tqdm
        )  # vLLM 支援多 prompt 併發 :contentReference[oaicite:0]{index=0}
        return [o.outputs[0].text for o in outs]


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


def get_main_cpp() -> str:
    return """/*DO NOT MODIFY THIS FILE*/

#include <iostream>
#include <string>
using namespace std;

#ifndef NATIVE_SYSTEMC
#include "esc.h"
#endif

// Wall Clock Time Measurement
#include <sys/time.h>

#include "SystemPipeline.h"

// TIMEVAL STRUCT IS Defined ctime
// use start_time and end_time variables to capture
// start of simulation and end of simulation
struct timeval start_time, end_time;

SystemPipeline * sys = NULL;

#ifndef NATIVE_SYSTEMC
extern void esc_elaborate()
{
	sys = new SystemPipeline("sys");
}
extern void esc_cleanup()
{
	delete sys;
}
#endif

// int main(int argc, char *argv[])
int sc_main(int argc, char **argv) {

#ifndef NATIVE_SYSTEMC
	esc_initialize(argc, argv);
#endif
	
#ifndef NATIVE_SYSTEMC
	esc_elaborate();
#else
	sys = new SystemPipeline("sys");
#endif
	sc_start();
#ifndef NATIVE_SYSTEMC
	esc_cleanup();
#else
	delete sys;
#endif
	std::cout<< "Simulated time == " << sc_core::sc_time_stamp() << std::endl;

  return 0;
}
"""


def get_testcases(task: str) -> list[str]:
    with open(os.path.join(INPUT_DATA_DIR, task, "testcases.txt")) as f:
        return f.read()


def get_golden(task: str) -> str:
    with open(os.path.join(INPUT_DATA_DIR, task, "golden.txt")) as f:
        return f.read()


def get_testcases_and_golden(task: str) -> None:
    testcases = get_testcases(task).splitlines()
    golden = get_golden(task).splitlines()
    txt: list[str] = []

    for line_tc, line_gn in zip(testcases, golden):
        if line_tc.startswith("#"):
            continue
        line_tc = line_tc.strip()
        line_gn = line_gn.strip()

        txt.append(f"{line_tc}  {line_gn}")

    return "\n".join(txt)


if __name__ == "__main__":
    # model_name = "Qwen/Qwen2.5-Coder-7B-Instruct"
    # generator = VLLMGenerator(model_name)

    # prompt = "Write a Python function to calculate the factorial of a number."
    # print(generator(prompt))
    get_testcases_and_golden("FindGCD")
