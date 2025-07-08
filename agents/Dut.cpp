INFO 07-08 02:34:20 [__init__.py:239] Automatically detected platform cuda.
Using 2 GPU(s) via tensor-parallelism
INFO 07-08 02:34:26 [config.py:600] This model supports multiple tasks: {'generate', 'embed', 'score', 'reward', 'classify'}. Defaulting to 'generate'.
INFO 07-08 02:34:26 [config.py:1600] Defaulting to use mp for distributed inference
INFO 07-08 02:34:26 [config.py:1780] Chunked prefill is enabled with max_num_batched_tokens=8192.
INFO 07-08 02:34:27 [core.py:61] Initializing a V1 LLM engine (v0.8.3) with config: model='Qwen/Qwen2.5-Coder-7B-Instruct', speculative_config=None, tokenizer='Qwen/Qwen2.5-Coder-7B-Instruct', skip_tokenizer_init=False, tokenizer_mode=auto, revision=None, override_neuron_config=None, tokenizer_revision=None, trust_remote_code=True, dtype=torch.bfloat16, max_seq_len=32768, download_dir=None, load_format=auto, tensor_parallel_size=2, pipeline_parallel_size=1, disable_custom_all_reduce=False, quantization=None, enforce_eager=False, kv_cache_dtype=auto,  device_config=cuda, decoding_config=DecodingConfig(guided_decoding_backend='xgrammar', reasoning_backend=None), observability_config=ObservabilityConfig(show_hidden_metrics=False, otlp_traces_endpoint=None, collect_model_forward_time=False, collect_model_execute_time=False), seed=None, served_model_name=Qwen/Qwen2.5-Coder-7B-Instruct, num_scheduler_steps=1, multi_step_stream_outputs=True, enable_prefix_caching=True, chunked_prefill_enabled=True, use_async_output_proc=True, disable_mm_preprocessor_cache=False, mm_processor_kwargs=None, pooler_config=None, compilation_config={"level":3,"custom_ops":["none"],"splitting_ops":["vllm.unified_attention","vllm.unified_attention_with_output"],"use_inductor":true,"compile_sizes":[],"use_cudagraph":true,"cudagraph_num_of_warmups":1,"cudagraph_capture_sizes":[512,504,496,488,480,472,464,456,448,440,432,424,416,408,400,392,384,376,368,360,352,344,336,328,320,312,304,296,288,280,272,264,256,248,240,232,224,216,208,200,192,184,176,168,160,152,144,136,128,120,112,104,96,88,80,72,64,56,48,40,32,24,16,8,4,2,1],"max_capture_size":512}
WARNING 07-08 02:34:27 [multiproc_worker_utils.py:306] Reducing Torch parallelism from 24 threads to 1 to avoid unnecessary CPU contention. Set OMP_NUM_THREADS in the external environment to tune this value as needed.
INFO 07-08 02:34:27 [shm_broadcast.py:264] vLLM message queue communication handle: Handle(local_reader_ranks=[0, 1], buffer_handle=(2, 10485760, 10, 'psm_533820c5'), local_subscribe_addr='ipc:///tmp/f5cf0dd3-9cd6-41ed-aa68-04a339a554cd', remote_subscribe_addr=None, remote_addr_ipv6=False)
WARNING 07-08 02:34:28 [utils.py:2413] Methods determine_num_available_blocks,device_config,get_cache_block_size_bytes,initialize_cache not implemented in <vllm.v1.worker.gpu_worker.Worker object at 0x76677b9dd990>
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [shm_broadcast.py:264] vLLM message queue communication handle: Handle(local_reader_ranks=[0], buffer_handle=(1, 10485760, 10, 'psm_9751bd69'), local_subscribe_addr='ipc:///tmp/ab6ea74c-2fc5-4484-8941-adb555d54228', remote_subscribe_addr=None, remote_addr_ipv6=False)
WARNING 07-08 02:34:28 [utils.py:2413] Methods determine_num_available_blocks,device_config,get_cache_block_size_bytes,initialize_cache not implemented in <vllm.v1.worker.gpu_worker.Worker object at 0x76677b9dd030>
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [shm_broadcast.py:264] vLLM message queue communication handle: Handle(local_reader_ranks=[0], buffer_handle=(1, 10485760, 10, 'psm_c446bc65'), local_subscribe_addr='ipc:///tmp/4227c4bd-27ba-4637-958b-8bddffd4cc7a', remote_subscribe_addr=None, remote_addr_ipv6=False)
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [utils.py:990] Found nccl from library libnccl.so.2
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [utils.py:990] Found nccl from library libnccl.so.2
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [pynccl.py:69] vLLM is using nccl==2.25.1
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [pynccl.py:69] vLLM is using nccl==2.25.1
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [custom_all_reduce_utils.py:244] reading GPU P2P access cache from /home/S113062628/.cache/vllm/gpu_p2p_access_cache_for_0,1.json
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [custom_all_reduce_utils.py:244] reading GPU P2P access cache from /home/S113062628/.cache/vllm/gpu_p2p_access_cache_for_0,1.json
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [shm_broadcast.py:264] vLLM message queue communication handle: Handle(local_reader_ranks=[1], buffer_handle=(1, 4194304, 6, 'psm_39929dab'), local_subscribe_addr='ipc:///tmp/45475615-1d1d-41f0-a4d1-11be83e80258', remote_subscribe_addr=None, remote_addr_ipv6=False)
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [parallel_state.py:957] rank 0 in world size 2 is assigned as DP rank 0, PP rank 0, TP rank 0
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [parallel_state.py:957] rank 1 in world size 2 is assigned as DP rank 0, PP rank 0, TP rank 1
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [cuda.py:221] Using Flash Attention backend on V1 engine.
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [cuda.py:221] Using Flash Attention backend on V1 engine.
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [gpu_model_runner.py:1258] Starting to load model Qwen/Qwen2.5-Coder-7B-Instruct...
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [gpu_model_runner.py:1258] Starting to load model Qwen/Qwen2.5-Coder-7B-Instruct...
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:28 [topk_topp_sampler.py:44] Currently, FlashInfer top-p & top-k sampling sampler is disabled because FlashInfer>=v0.2.3 is not backward compatible. Falling back to the PyTorch-native implementation of top-p & top-k sampling.
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:28 [topk_topp_sampler.py:44] Currently, FlashInfer top-p & top-k sampling sampler is disabled because FlashInfer>=v0.2.3 is not backward compatible. Falling back to the PyTorch-native implementation of top-p & top-k sampling.
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:29 [weight_utils.py:265] Using model weights format ['*.safetensors']
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:29 [weight_utils.py:265] Using model weights format ['*.safetensors']
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:31 [loader.py:447] Loading weights took 1.47 seconds
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:31 [gpu_model_runner.py:1273] Model loading took 7.1217 GiB and 2.379969 seconds
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:31 [loader.py:447] Loading weights took 1.49 seconds
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:31 [gpu_model_runner.py:1273] Model loading took 7.1217 GiB and 2.859940 seconds
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:34 [backends.py:416] Using cache directory: /home/S113062628/.cache/vllm/torch_compile_cache/05f9697228/rank_1_0 for vLLM's torch.compile
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:34 [backends.py:426] Dynamo bytecode transform time: 3.09 s
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:34 [backends.py:416] Using cache directory: /home/S113062628/.cache/vllm/torch_compile_cache/05f9697228/rank_0_0 for vLLM's torch.compile
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:34 [backends.py:426] Dynamo bytecode transform time: 3.12 s
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:35 [backends.py:115] Directly load the compiled graph for shape None from the cache
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:35 [backends.py:115] Directly load the compiled graph for shape None from the cache
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:38 [monitor.py:33] torch.compile takes 3.09 s in total
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:38 [monitor.py:33] torch.compile takes 3.12 s in total
INFO 07-08 02:34:38 [kv_cache_utils.py:578] GPU KV cache size: 1,095,520 tokens
INFO 07-08 02:34:38 [kv_cache_utils.py:581] Maximum concurrency for 32,768 tokens per request: 33.43x
INFO 07-08 02:34:38 [kv_cache_utils.py:578] GPU KV cache size: 1,096,080 tokens
INFO 07-08 02:34:38 [kv_cache_utils.py:581] Maximum concurrency for 32,768 tokens per request: 33.45x
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:51 [custom_all_reduce.py:195] Registering 3819 cuda graph addresses
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:51 [custom_all_reduce.py:195] Registering 3819 cuda graph addresses
[1;36m(VllmWorker rank=1 pid=158695)[0;0m INFO 07-08 02:34:51 [gpu_model_runner.py:1608] Graph capturing finished in 12 secs, took 0.46 GiB
[1;36m(VllmWorker rank=0 pid=158677)[0;0m INFO 07-08 02:34:51 [gpu_model_runner.py:1608] Graph capturing finished in 12 secs, took 0.46 GiB
INFO 07-08 02:34:51 [core.py:162] init engine (profile, create kv cache, warmup model) took 19.73 seconds
```cpp
/*
Instructions:
Fix Format: DO NOT MODIFY THIS SECTION
Variable Section: There are 2 input port(s) (i_a, i_b) and 1 output port (o_result)
Main function Section: Modify the function of the DUT here.
*/

#include "Dut.h"

// {FUNC}

Dut::Dut(sc_module_name n) : sc_module(n) {
    /* === Fixed Format === */
    SC_THREAD(do_compute);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
    /* === Fixed Format End === */
}

void Dut::do_compute() {

    {
        wait();
    }

    while (true) {
        /* === Variable Section === */
        int a = i_a.read();
        int b = i_b.read();
        /* === Variable Section End === */

        /* === Main function Section === */
        int res = add(a, b);
        /* === Main function Section End === */

        /* === Variable Section === */
        o_result.write(res);
        /* === Variable Section End === */
    }
}
```
