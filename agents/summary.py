from typing import Dict, List, Union
from utils import VLLMGenerator
from pathlib import Path
import re

_SUMMARY_SYSTEM_PROMPT = """You are a senior SystemC/C++ verification engineer with 10+ years of experience debugging simulation issues.
You are skilled at analyzing compilation/runtime error logs and tracing issues to specific modules (DUT, Testbench, Pipeline).

Task
----
Read the six source files (Dut / Testbench / SystemPipeline, .h/.cpp) and the terminal outputs.
Return EXACTLY two lines, no extra explanations.
[REFINE] <dut|testbench|pipeline|comma_separated> [/REFINE]
[SUGGEST] <short, clear suggestion how to fix> [/SUGGEST]
"""
_FILE_FMT = """** FILE: {name} **
```cpp
{code}
```"""
_OUTPUT_FMT = """** OUTPUTS: **
stdout: {stdout}
stderr: {stderr}
returncode: {returncode}
127 means timeout.
"""
_REFINE_PAT = re.compile("\\[REFINE\\]\\s*([a-z,]+)\\s*\\[/REFINE\\]", re.I)
_SUGGEST_PAT = re.compile("\\[SUGGEST\\]\\s*(.+?)\\s*\\[/SUGGEST\\]", re.I | re.S)


def _count_rounds(total: int, batch_size: int) -> int:
    remainder = total % batch_size
    if remainder == 0:
        return total // batch_size
    return total // batch_size + 1


def _parse_refine(raw: str) -> List[str]:
    m = _REFINE_PAT.search(raw)
    agents = [s.strip().lower() for s in m.group(1).split(",")]
    return agents


def _parse_suggest(raw: str) -> str:
    m = _SUGGEST_PAT.search(raw)
    return m.group(1).strip()


class SummaryAgent:
    def __init__(
        self,
        model: VLLMGenerator,
        temperature: float,
        top_p: float,
        max_new_tokens: int,
    ):
        self.model = model
        self.temperature = temperature
        self.top_p = top_p
        self.max_new_tokens = max_new_tokens
        self.database = []

    def add_data(self, error_msg: Dict[str, Dict]):
        for qname, data in error_msg.items():
            entry = {
                "qname": qname,
                "stdout": data.get("stdout", ""),
                "stderr": data.get("stderr", ""),
                "returncode": data.get("returncode", -1),
            }
            self.database.append(entry)

    def empty_data(self):
        self.database = []

    def summarize(self, path: Path, batch_size: int = 16) -> Dict:
        prompts = []
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
            else:
                prompt = [
                    {"role": "system", "content": _SUMMARY_SYSTEM_PROMPT},
                    {
                        "role": "user",
                        "content": blocks + "\n\n" + _OUTPUT_FMT.format(**data),
                    },
                ]
                prompt = self.model.apply_chat_template(
                    prompt, tokenize=False, add_generation_prompt=True
                )
                prompts.append(prompt)

        rounds = _count_rounds(len(self.database), batch_size)
        agents = []
        summaries = []
        for i in range(rounds):
            batch_prompts = prompts[i * batch_size : (i + 1) * batch_size]
            responses = self.model.generate_batch(
                batch_prompts,
                temperature=self.temperature,
                top_p=self.top_p,
                max_new_tokens=self.max_new_tokens,
            )
            for resp in responses:
                agents.append(_parse_refine(resp))
                summaries.append(_parse_suggest(resp))
        return {
            self.database[i]["qname"]: {
                "agents": agents[i],
                "summary": summaries[i],
            }
            for i in range(len(self.database))
        }
