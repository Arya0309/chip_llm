from typing import Dict, List, Union
from utils import VLLMGenerator
from pathlib import Path
from utils import count_rounds
import re
import json

# Prompt v0.0
_SUMMARY_SYSTEM_PROMPT_V0_0 = """You are a senior SystemC/C++ verification engineer with experience debugging simulation issues.
You are skilled at analyzing compilation/runtime error logs and tracing issues to specific modules (DUT, Testbench, Pipeline).

Task
----
Read the six source files (Dut / Testbench / SystemPipeline, .h/.cpp) and the terminal outputs.
Return EXACTLY two lines, no extra explanations.
[REFINE] <dut|testbench|pipeline|comma_separated> [/REFINE]
[SUGGEST] <detailed and clear suggestion how to fix> [/SUGGEST]
"""
# Prompt v1.0
_SUMMARY_SYSTEM_PROMPT_V1_0 = """You are a senior SystemC/C++ verification engineer with experience debugging simulation issues.
You are skilled at analyzing compilation/runtime error logs and tracing issues to specific modules (DUT, Testbench, Pipeline).

Task
----
Read the six source files (Dut / Testbench / SystemPipeline, .h/.cpp) and the terminal outputs.
Return EXACTLY two sections, no extra explanations.
[REFINE] <dut|testbench|pipeline|comma_separated> [/REFINE]
[SUGGEST] <Provide a step-by-step modification plan.> [/SUGGEST]
"""
# Prompt v1.1
_SUMMARY_SYSTEM_PROMPT_V1_1 = """You are a senior SystemC/C++ verification engineer with experience debugging simulation issues.
You are skilled at analyzing compilation/runtime error logs and tracing issues to specific modules (DUT, Testbench, Pipeline).

Hard Rules
----------
1. Assume compile and run each have a hard 60-second limit. You must not suggest changing this limit. If logs indicate a timeout, say so explicitly and propose code/testbench/scheduling optimizations—not longer time limits.
2. No extra commentary, markdown, or explanations outside the two required sections.

Task
----
Read the six source files and the terminal outputs.
Return EXACTLY two sections, no extra explanations.
[REFINE] <dut|testbench|pipeline|comma_separated> [/REFINE]

Provide a step-by-step modification plan for each module listed in the [REFINE] section. Use this exact format:
[AGENT] <module_name> [/AGENT]
[SUGGEST] <a step-by-step modification plan> [/SUGGEST]"
"""

_FILE_FMT = """** FILE: {name} **
```cpp
{code}
```"""
_OUTPUT_FMT = """** OUTPUTS: **
stdout: {stdout}
stderr: {stderr}
returncode: {returncode}

Please summarize the possible causes of the error based on the above code and outputs, and suggest which modules (dut, testbench, pipeline) may need refinement.
Wrap module names between [REFINE] and [/REFINE], and wrap your suggestion between [SUGGEST] and [/SUGGEST].
And testcases.txt and golden.txt are always correct.
"""

_ALLOWED_AGENTS = {"dut", "testbench", "pipeline"}
_ALLOWED_PROMPT_MODES = {"prompt v0.0", "prompt v1.0", "prompt v1.1"}
_REFINE_PAT = re.compile(r"\[REFINE\]\s*([a-z,\s]+)\s*\[/REFINE\]", re.I | re.S)
_AGENT_PAT = re.compile(r"\[AGENT\]\s*(.+?)\s*\[/AGENT\]", re.I | re.S)
_SUGGEST_PAT = re.compile(r"\[SUGGEST\]\s*(.+?)\s*\[/SUGGEST\]", re.I | re.S)


def _build_summary_prompt_with_code(
    blocks: str, data: Dict[str, Union[str, int]], mode: str
) -> List[Dict]:
    if mode == "prompt v0.0":
        system_prompt = _SUMMARY_SYSTEM_PROMPT_V0_0
    elif mode == "prompt v1.0":
        system_prompt = _SUMMARY_SYSTEM_PROMPT_V1_0
    elif mode == "prompt v1.1":
        system_prompt = _SUMMARY_SYSTEM_PROMPT_V1_1
    return [
        {"role": "system", "content": system_prompt},
        {"role": "user", "content": blocks + "\n\n" + _OUTPUT_FMT.format(**data)},
    ]


def _parse_refine(raw: str) -> List[str]:
    m = _REFINE_PAT.search(raw)
    if not m:
        raise ValueError("[REFINE] tag not found.")
    agents = [s.strip().lower() for s in m.group(1).split(",")]
    agents = [a for a in agents if a in _ALLOWED_AGENTS]
    if not agents:
        raise ValueError("No valid agents")
    return agents


# Prompt v0.0 and v1.0
def _parse_suggest(raw: str) -> List[str]:
    m = _SUGGEST_PAT.search(raw)
    if not m:
        raise ValueError("[SUGGEST] tag not found.")
    return m.group(1).strip()


# Prompt v1.1
def _parse_suggest_pair(raw: str) -> Dict[str, str]:

    def _mapping(agent_name: str) -> str:
        if agent_name == "systempipeline":
            return "pipeline"
        return agent_name

    pairs = {}
    matches_agent = list(_AGENT_PAT.finditer(raw))
    matches_suggest = list(_SUGGEST_PAT.finditer(raw))
    if len(matches_agent) != len(matches_suggest):
        raise ValueError("Mismatched number of [AGENT] and [SUGGEST] tags.")
    for agent, suggest in zip(matches_agent, matches_suggest):
        agent_name = agent.group(1).strip().lower()
        agent_name = _mapping(agent_name)
        suggest_text = suggest.group(1).strip()
        if agent_name not in _ALLOWED_AGENTS:
            raise ValueError(f"Invalid agent name: {agent_name}")
        pairs[agent_name] = suggest_text
    return pairs


def write_json(path: Path, data: Dict):
    path.write_text(json.dumps(data, indent=2), "utf-8")


class SummaryAgent:
    def __init__(
        self,
        *,
        model: VLLMGenerator,
        temperature: float,
        top_p: float,
        max_new_tokens: int,
        mode: str,
    ):
        self.model = model
        self.temperature = temperature
        self.top_p = top_p
        self.max_new_tokens = max_new_tokens
        self.database = []
        if mode not in _ALLOWED_PROMPT_MODES:
            raise ValueError(f"Invalid prompt mode: {mode}")
        self.mode = mode

    def add_data(self, error_msg: Dict[str, Dict]):
        for qname, data in error_msg.items():
            entry = {
                "qname": qname,
                "stdout": data.get("stdout", ""),
                "stderr": data.get("stderr", ""),
                "returncode": data.get("returncode", -1),
                "prompt": None,
                "raw_response": None,
                "agents": None,
                "summary": None,
                "done": False,
            }
            self.database.append(entry)

    def empty_data(self):
        self.database = []

    def summarize(self, path: Path, batch_size: int = 16) -> Dict:
        write_path = path / "summary.json"
        code_files = [
            "Dut.h",
            "Dut.cpp",
            "Testbench.h",
            "Testbench.cpp",
            "SystemPipeline.h",
            "SystemPipeline.cpp",
        ]
        for data in self.database:
            qname_path = path / data["qname"]
            blocks = ""
            for f in code_files:
                if not (qname_path / f).exists():
                    raise FileNotFoundError(f"File not found: {qname_path / f}")
                code = (qname_path / f).read_text("utf-8")
                blocks += _FILE_FMT.format(name=f, code=code) + "\n\n"

                prompt = _build_summary_prompt_with_code(blocks, data, self.mode)
                prompt = self.model.apply_chat_template(
                    prompt, tokenize=False, add_generation_prompt=True
                )
                data["prompt"] = prompt

        tryouts = 20
        all_done = False
        temperature_temp = self.temperature
        for i in range(tryouts):
            print(f"Generating summaries... (Attempt {i + 1})")
            if i > 2:
                # 為了成功生成 summary，嘗試提高 temperature
                self.temperature = min(self.temperature + (i - 2) * 0.2, 1.0)
            all_done = self._generate_summary(batch_size)
            if all_done:
                break
        self.temperature = temperature_temp

        write_dataset = {
            data["qname"]: {
                "agents": data["agents"],
                "summary": data["summary"],
                "raw_response": data["raw_response"],
            }
            for data in self.database
        }
        write_json(write_path, write_dataset)

        if not all_done:
            # raise RuntimeError(f"Failed to generate summaries after {tryouts} attempts.")
            print(f"Warning: Failed to generate summaries after {tryouts} attempts.")

        return {
            data["qname"]: {
                "agents": data["agents"],
                "summary": data["summary"],
            }
            for data in self.database
        }

    def _generate_summary(self, batch_size: int = 16) -> bool:
        # 不要取已經完成 summary 的 data
        # 這邊 pending_data 會修改 self.database 裡的內容，因為是 reference
        pending_data = [data for data in self.database if not data["done"]]
        if not pending_data:
            return True

        rounds = count_rounds(len(pending_data), batch_size)
        prompts = [data["prompt"] for data in pending_data]
        responses = []

        for i in range(rounds):
            batch_prompts = prompts[i * batch_size : (i + 1) * batch_size]
            batch_responses = self.model.generate_batch(
                batch_prompts,
                temperature=self.temperature,
                top_p=self.top_p,
                max_new_tokens=self.max_new_tokens,
            )
            responses.extend(batch_responses)

        for data, resp in zip(pending_data, responses):
            data["raw_response"] = resp
            data["agents"] = None
            data["summary"] = None

            if self.mode in ("prompt v0.0", "prompt v1.0"):
                try:
                    data["agents"] = _parse_refine(resp)
                    data["summary"] = _parse_suggest(resp)
                except Exception:
                    pass
            elif self.mode in ("prompt v1.1"):
                try:
                    data["agents"] = _parse_refine(resp)
                    data["summary"] = _parse_suggest_pair(resp)
                except Exception:
                    pass

            data["done"] = data["agents"] is not None and data["summary"] is not None

        return all(data["done"] for data in self.database)
