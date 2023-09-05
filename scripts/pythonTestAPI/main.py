import json
import sseclient
import requests

url = 'http://127.0.0.1:8080/completion'

data_context = {
    "prompt": "### Human: 什么是脉冲星 "
              "### Assistant: 脉冲星是一种特殊的恒星，它们会以极快的速度旋转。它们的表面温度极高，可达到数百万度以上。脉冲星的表面温度比太阳高得多，这使得它们能够发出强烈的辐射。"
              "### Human: 什么是恒星 "
              "### Assistant: ",
    "temperature": 0.2,
    "top_k": 40,
    "top_p": 0.9,
    "n_keep": 36,
    "n_predict": 2048,
    "stop": ["### Human:"],
    "stream": True
}
data_context_json = json.dumps(data_context)
hearder_context = {"Content-Type": "application/json"}

sess = requests.session()

with sess.post(url, stream=True, headers=hearder_context, data=data_context_json) as resp:
    for line in resp.iter_lines():
        if line:
            result_item = line.decode('utf-8').replace("data: ", "")
            item_json_obj = json.loads(result_item)
            if not item_json_obj['stop']:
                item_content = item_json_obj['content']
                print(item_content, end='')
            else:
                item_timings = item_json_obj['timings']
                print('\n')
                print(item_timings)
                print("predicted_ms = {}".format(item_timings['predicted_ms']))
                print("predicted_per_token_ms = {}".format(item_timings['predicted_per_token_ms']))
                print("prompt_ms = {}".format(item_timings['prompt_ms']))
                print("prompt_per_token_ms = {}".format(item_timings['prompt_per_token_ms']))

