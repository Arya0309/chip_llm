# utils.py
from __future__ import annotations
from pathlib import Path
import os
import random

from typing import Any, Dict
import torch
from vllm import LLM, SamplingParams
from transformers import AutoTokenizer, AutoModelForCausalLM
import concurrent.futures # 新增：用於 OpenAI 並行請求
from openai import OpenAI

DEFAULT_MODEL = os.getenv("LLM_MODEL", "Qwen/Qwen2.5-Coder-32B-Instruct")
#DEFAULT_MODEL = os.getenv("LLM_MODEL", "openai/gpt-oss-20b")
#DEFAULT_MODEL = os.getenv("LLM_MODEL", "gpt-4o")
PROJECT_ROOT = Path(__file__).resolve().parents[1]  # /home/.../chip_llm
INPUT_DATA_DIR = PROJECT_ROOT / "data_inputs_new"  # /home/.../chip_llm/data_inputs


class HFGenerator:
    """HuggingFace Transformers-based generator for LLM tasks."""

    def __init__(self, model_name: str = DEFAULT_MODEL):
        self.tokenizer = AutoTokenizer.from_pretrained(model_name)
        self.model = AutoModelForCausalLM.from_pretrained(model_name).to("cuda")

    def apply_chat_template(
        self,
        messages,
        *,
        tokenize: bool = False,
        add_generation_prompt: bool = True,
    ):
        if tokenize:  # 用在 generate() 時
            return self.tokenizer.apply_chat_template(
                messages,
                tokenize=True,
                add_generation_prompt=add_generation_prompt,
                return_dict=True,
                return_tensors="pt",
            ).to(self.model.device)
        else:  # 用在 _chat_to_prompt() 時
            return self.tokenizer.apply_chat_template(
                messages,
                tokenize=False,
                add_generation_prompt=add_generation_prompt,
            )

    def generate(self, prompt: str, **generate_kwargs) -> str:
        # 1) 先把 chat 轉成 token 張量
        inputs = self.apply_chat_template(
            [{"role": "user", "content": prompt}], tokenize=True
        )

        # 2) 過濾 vLLM only 參數
        for bad in ("use_tqdm", "stream", "stop", "stop_token_ids", "echo"):
            generate_kwargs.pop(bad, None)

        # 3) 交給 HF generate()
        outputs = self.model.generate(
            **inputs,
            **generate_kwargs,  # temperature / top_p / top_k…
        )

        # 4) 去掉 prompt token，回傳文字
        #return self.tokenizer.decode(
        #    outputs[0][inputs["input_ids"].shape[-1] :], skip_special_tokens=False
        #)
        response = self.tokenizer.decode(
            outputs[0][inputs["input_ids"].shape[-1] :], skip_special_tokens=False
        )
        return response.split("</think>")[-1] if "</think>" in response else response


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
        #return outputs[0].outputs[0].text
        response = outputs[0].outputs[0].text
        return response.split("</think>")[-1] if "</think>" in response else response

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
        #return [o.outputs[0].text for o in outs]
        responses = [o.outputs[0].text for o in outs]
        return [r.split("</think>")[-1] if "</think>" in r else r for r in responses]


# ===========================================================================
# 新增：OpenAI Generator (介面模仿 VLLMGenerator)
# ===========================================================================
class OpenAIGenerator:
    def __init__(self, model_name: str = DEFAULT_MODEL, **kwargs):
        if OpenAI is None:
            raise ImportError("Please install openai: pip install openai")
        
        # 從環境變數讀取 OPENAI_API_KEY
        api_key = os.getenv("OPENAI_API_KEY")
        base_url = os.getenv("OPENAI_BASE_URL") # 支援自訂 Endpoint (如 vLLM server, Azure)
        
        if not api_key:
            print("Warning: OPENAI_API_KEY not found in env.")
            
        self.client = OpenAI(api_key=api_key, base_url=base_url)
        self.model_name = model_name
        print(f"[OpenAIGenerator] Initialized for model: {model_name}")

    def apply_chat_template(
        self,
        messages,
        *,
        tokenize: bool = False,
        add_generation_prompt: bool = True,
    ):
        """
        Override: 不進行 tokenization 或 string format。
        直接回傳 messages list，讓 generate_batch 直接傳給 OpenAI API。
        """
        return messages

    def generate(
        self,
        prompt: Union[str, List[Dict]],
        *,
        max_new_tokens: int = 4096,
        temperature: float = 0.3,
        top_p: float = 0.8,
        **kwargs,
    ) -> str:
        # 如果傳入的是 list (messages)，直接用；如果是 str，包裝成 user message
        messages = prompt if isinstance(prompt, list) else [{"role": "user", "content": prompt}]

        try:
            response = self.client.chat.completions.create(
                model=self.model_name,
                messages=messages,
                temperature=temperature,
                max_tokens=max_new_tokens,
                top_p=top_p,
            )
            content = response.choices[0].message.content
            return self._clean_response(content)
        except Exception as e:
            print(f"[OpenAI Error] {e}")
            return ""

    # 允許像函式一樣直接呼叫
    __call__ = generate

    def generate_batch(
        self,
        prompts: list[Union[str, List[Dict]]],
        *,
        max_new_tokens: int = 4096,
        temperature: float = 0.3,
        top_p: float = 0.8,
        **kwargs,
    ) -> list[str]:
        """
        使用 ThreadPoolExecutor 並行呼叫 OpenAI API 來模擬 Batch 處理。
        """
        def _call_api(single_prompt):
            return self.generate(
                single_prompt,
                max_new_tokens=max_new_tokens,
                temperature=temperature,
                top_p=top_p,
            )

        # 建議根據 API Rate Limit 調整 max_workers
        max_workers = int(os.getenv("OPENAI_MAX_WORKERS", "8"))
        
        results = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            # 使用 map 確保回傳順序與輸入順序一致
            results = list(executor.map(_call_api, prompts))
            
        return results

    def _clean_response(self, response: str) -> str:
        """處理 deepseek/qwen 等可能出現的思考標籤"""
        if response and "</think>" in response:
            return response.split("</think>")[-1]
        return response

# ===========================================================================
# 新增：Factory Function (統一入口)
# ===========================================================================
def LLMGeneratorFactory(model_name: str = DEFAULT_MODEL) -> Union[VLLMGenerator, OpenAIGenerator]:
    """
    根據 model_name 決定回傳 VLLMGenerator 或 OpenAIGenerator。
    """
    lower_name = model_name.lower()
    
    # 判斷邏輯：如果是 GPT 系列或 o1 系列，使用 OpenAI Generator
    openai_keywords = ["gpt-", "o1-", "o3-", "text-embedding"]
    
    if any(k in lower_name for k in openai_keywords):
        return OpenAIGenerator(model_name)
    else:
        # 預設還是走 vLLM (本地模型)
        return VLLMGenerator(model_name)
        
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


def count_rounds(total: int, batch_size: int) -> int:
    remainder = total % batch_size
    if remainder == 0:
        return total // batch_size
    return total // batch_size + 1


if __name__ == "__main__":
    # model_name = "Qwen/Qwen2.5-Coder-7B-Instruct"
    # generator = VLLMGenerator(model_name)

    # prompt = "Write a Python function to calculate the factorial of a number."
    # print(generator(prompt))
    get_testcases_and_golden("FindGCD")
