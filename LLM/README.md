LLama-chinese project
============================================
## references and tips
1. 清华开源镜像站
```bash
https://mirrors.tuna.tsinghua.edu.cn/help/pypi/
pip install -i https://pypi.tuna.tsinghua.edu.cn/simple some-package
```
2. 内存不足
参考[此处](https://huggingface.co/OpenAssistant/oasst-sft-6-llama-30b-xor/discussions/4)扩充swap
```bash
#check your current swap size
free -h
#turn off your current swap
sudo swapoff -a
#increase swap to 100GB to be able to offload the entire model from RAM to disk
sudo fallocate -l 100G /swapfile
#make sure swapfile permissions are set, then activate
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
#check new swap size (should say something like 97Gi)
free -h
```
3. WSL最大硬盘空间限制是256GB

## steps
1. download models
    1. download original llama models by `download_llama.sh 7B`
    2. download chinese models: `Chinese-LLaMA-Alpaca/README_EN.md`
    3. create `Chinese-LLaMA-Alpaca/models`, mov all chinese models zip files into and unzip to sub-folders with same name as zips files.
    4. check sha256sum with `Chinese-LLaMA-Alpaca/SHA256.md`, re-download the error files

2. convert original llama models to hf format
```bash
cd transformers
python src/transformers/models/llama/convert_llama_weights_to_hf.py \
    --input_dir path_to_original_llama_root_dir \
    --model_size 7B \
    --output_dir path_to_original_llama_hf_dir
```

3. merge original llama models and chinese models
```bash.
cd Chinese-LLaMA-Alpaca
python scripts/merge_llama_with_chinese_lora_low_mem.py \
    --base_model path_to_original_llama_hf_dir \
    --lora_model path_to_chinese_llama_plus_lora,path_to_chinese_alpaca_plus_lora \
    --output_type [pth|huggingface] \
    --output_dir path_to_merge_output_dir 
```
注意，此法生成的所有的模型的tokenizer.model是一样的,无论是7B,13B

4. compile llama.cpp
```bash
cd llama.cpp
make -j $(nproc)
```

5. llama.cpp inference
    1. 转移文件
    ```bash
    mkdir llama.cpp/zh-models
    mkdir llama.cpp/zh-models/7B
    cp path_to_7B_merge_output_dir/*.pth llama.cpp/zh-models/7B/
    cp path_to_7B_merge_output_dir/params.json llama.cpp/zh-models/7B/
    cp path_to_7B_merge_output_dir/tokenizer.model llama.cpp/zh-models/
    ```
    2. 量化到fp16
    cd llama.cpp
    python convert.py zh-models/7B/
    3. 量化到int4
    ./quantize ./zh-models/7B/ggml-model-f16.bin ./zh-models/7B/ggml-model-q4_0.bin q4_0
    4. inference
    ./main -m zh-models/7B/ggml-model-q4_0.bin --color -f prompts/alpaca.txt -ins -c 2048 --temp 0.2 -n 256 --repeat_penalty 1.1


## perf
* 原始llama + chinese_llama_plus_lora_7b + chinese_alpaca_pro_lora_7b, q4_0, CPU：10600KF, Memory: 64GB, 2666Mhz<br>
```
llama_print_timings:        load time = 70090.63 ms
llama_print_timings:      sample time =   787.54 ms /  1282 runs   (    0.61 ms per token,  1627.86 tokens per second)
llama_print_timings: prompt eval time = 62803.78 ms /   417 tokens (  150.61 ms per token,     6.64 tokens per second)
llama_print_timings:        eval time = 263147.21 ms /  1281 runs   (  205.42 ms per token,     4.87 tokens per second)
llama_print_timings:       total time = 1307641.49 ms
```

* 原始llama + chinese_llama_plus_lora_13b + chinese_alpaca_pro_lora_13b, q4_0, CPU：12900K, Memory: 64GB, 2666Mhz<br>
llama_print_timings:        load time =  4343.98 ms
llama_print_timings:      sample time =   123.64 ms /   249 runs   (    0.50 ms per token,  2013.86 tokens per second)
llama_print_timings: prompt eval time = 16363.49 ms /   152 tokens (  107.65 ms per token,     9.29 tokens per second)
llama_print_timings:        eval time = 59542.70 ms /   248 runs   (  240.09 ms per token,     4.17 tokens per second)
llama_print_timings:       total time = 676802.44 ms

* 原始llama + chinese_llama_plus_lora_7b + chinese_alpaca_pro_lora_7b, q4_0, CPU：12900K, Memory: 64GB, 2666Mhz<br>
llama_print_timings:        load time =  5007.53 ms
llama_print_timings:      sample time =   176.25 ms /   362 runs   (    0.49 ms per token,  2053.89 tokens per second)
llama_print_timings: prompt eval time =  6246.96 ms /   116 tokens (   53.85 ms per token,    18.57 tokens per second)
llama_print_timings:        eval time = 47159.00 ms /   362 runs   (  130.27 ms per token,     7.68 tokens per second)
llama_print_timings:       total time = 127115.96 ms

13B模型的回答效果比7B好很多。