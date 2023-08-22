import json

import requests

url = 'http://127.0.0.1:8080/completion'

data_context = {
  "prompt": "### Human: 什么是奥本海默极限"
            "### Assistant: ",
  "temperature": 0.2,
  "top_k": 40,
  "top_p": 0.9,
  "n_keep": 36,
  "n_predict": 256,
  "stop": ["### Human:"],
  "stream": True
}
data_context_json = json.dumps(data_context)

# 以字典的形式构造数据
hearder_context = {"Content-Type": "application/json"}
# 与 get 请求一样，r 为响应对象
r = requests.post(url, data=data_context_json, headers=hearder_context)
# 查看响应结果

results = r.content.decode(encoding="utf8")
tokens_result = results.replace("data: ", "").split("\n\n")
tokens_result.pop()
answer = ''
for item in tokens_result:
    item_json_obj = json.loads(item)
    item_content = item_json_obj['content']
    if item_content != '':
        answer += item_content
print(answer.lstrip().rstrip())

# json.loads(results.replace("data: ", "").split("\n\n")[1])['content']
#
#
#
# print(r.content.decode(encoding="utf8"))