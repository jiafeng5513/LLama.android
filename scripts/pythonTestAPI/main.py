import json

import requests

url = 'http://127.0.0.1:8080/completion/'

data_context = {
    "prompt": "### Human: 那么什么是脉冲星呢### Assistant: 脉冲星是一种特殊的恒星，它具有非常强的磁场和辐射。### Human: 什么是恒星### Assistant:恒星是宇宙中发光的天体，它们通过核聚变产生能量并释放光线### Human: 什么是核聚变### Assistant:核聚变是一种将原子核合并成更重元素的过程，这是恒星的核心产生的能量来源### Human: 如果恒星的核聚变停止了会发生什么### Assistant: 恒星的核聚变通常持续数百万年，直到恒星耗尽其核心中的氢和氦。一旦核聚变停止，恒星将逐渐冷却并变成红巨星或白矮星。### Human: 恒星核聚变停止后，到底是成为白矮星还是中子星呢### Assistant: 恒星的核聚变通常持续数百万年，直到恒星耗尽其核心中的氢和氦。一旦核聚变停止，恒星将逐渐冷却并变成红巨星或白矮星。### Human: 什么是奥本海默极限### Assistant: ",
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
# headers = {'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36'}
# 与 get 请求一样，r 为响应对象
r = requests.post(url, data=data_context_json, headers=hearder_context)
# 查看响应结果
print(r)

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