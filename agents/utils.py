import torch
from vllm import LLM, SamplingParams
from transformers import AutoTokenizer


class VLLMGenerator:
    def __init__(self, model_name: str, tp_size: int | None = None):
        tp_size = tp_size or torch.cuda.device_count() or 1
        print(f"Using {tp_size} GPU(s) via tensor-parallelism")

        self.tokenizer = AutoTokenizer.from_pretrained(
            model_name,
            trust_remote_code=True,
        )
        self.llm = LLM(
            model=model_name,
            tensor_parallel_size=tp_size,
            trust_remote_code=True,
            dtype="auto",
        )

    def apply_chat_template(self, messages, tokenize=False, add_generation_prompt=True):
        return self.tokenizer.apply_chat_template(
            messages,
            tokenize=tokenize,
            add_generation_prompt=add_generation_prompt,
        )

    def generate(
        self,
        prompt: str,
        max_new_tokens: int = 1024,
        temperature: float = 0.7,
        top_p: float = 0.8,
        top_k: int = 20,
        repetition_penalty: float = 1.05,
    ) -> str:

        sampling_params = SamplingParams(
            max_tokens=max_new_tokens,
            temperature=temperature,
            top_p=top_p,
        )

        outputs = self.llm.generate([prompt], sampling_params)

        return outputs[0].outputs[0].text

    __call__ = generate


if __name__ == "__main__":
    model_name = "Qwen/Qwen2.5-Coder-7B-Instruct"
    generator = VLLMGenerator(model_name)

    prompt = "Write a Python function to calculate the factorial of a number."
    print(generator(prompt))
