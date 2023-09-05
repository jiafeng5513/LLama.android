//
// Created by anna on 23-8-22.
//

#include "ChatRequest.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#include "json.hpp"
#include "HTTPRequest.hpp"

using json = nlohmann::json;

ChatRequest::ChatRequest(QObject *parent) {
    requestPayload_["prompt"] = "### Human: 那么什么是脉冲星呢"
                                "### Assistant: 脉冲星是一种特殊的恒星，它具有非常强的磁场和辐射。"
                                "### Human: 什么是恒星"
                                "### Assistant:恒星是宇宙中发光的天体，它们通过核聚变产生能量并释放光线。"
                                "### Human: 什么是核聚变"
                                "### Assistant:核聚变是一种将原子核合并成更重元素的过程，这是恒星的核心产生的能量来源"
                                "### Human: 如果恒星的核聚变停止了会发生什么"
                                "### Assistant: 恒星的核聚变通常持续数百万年，直到恒星耗尽其核心中的氢和氦。一旦核聚变停止，恒星将逐渐冷却并变成红巨星或白矮星。";
    requestPayload_["temperature"] = 0.2;
    requestPayload_["top_k"] = 40;
    requestPayload_["top_p"] = 0.9;
    requestPayload_["n_keep"] = 36;
    requestPayload_["n_predict"] = 256;
    requestPayload_["stop"] = std::vector<std::string>({"### Human:"});
    requestPayload_["stream"] = true;
}

ChatRequest::~ChatRequest() {

}

void ChatRequest::run() {
    // todo: creat prompt.json with this->prompts

    // 以utf8读取文件，直接构造post请求，返回的response使用GBK解码显示
    try {
        http::Request request{"http://127.0.0.1:8080/completion"};
        std::stringstream sss;
        sss << requestPayload_;

//        const std::string body = UTF8ToGB(sss.str().c_str());
        const std::string body = sss.str();
        const auto response = request.send("POST", body, {{"Content-Type", "application/json"}});

        auto results = std::string{response.body.begin(), response.body.end()};
//        std::cout << results << std::endl;
        std::vector<std::string> sv;
        split(results, sv, '\n');
        std::stringstream ssr;
//        std::regex word_regex("content: (\\w+),");
        for (int i = 0; i < sv.size(); i++) {
            if (not sv[i].empty()) {
                auto payload = sv[i].substr(6, sv[i].length() - 6);  //

                json temp = json::parse(payload);
                auto content = temp["content"].get<std::string>();
//                auto content_gbk = UTF8ToGB(content.c_str());
                ssr << content;
                emit newResponse(QString::fromStdString(content));
            }
        }
        this->answer_ = ssr.str();
        // todo: 给payload 记录上下文
        requestPayload_["prompt"] = requestPayload_["prompt"].get<std::string>() + this->answer_;
    }
    catch (const std::exception &e) {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }

    emit requestReturn();
}

void ChatRequest::sendPrompt(const std::string &prompt) {
    requestPayload_["prompt"] = requestPayload_["prompt"].get<std::string>() + " ### Human: " + prompt + " ### Assistant: ";
}

std::string ChatRequest::getAnswer() {
    return this->answer_;
}

void ChatRequest::split(const std::string &s, std::vector<std::string> &sv, const char delim) {
    sv.clear();
    std::istringstream iss(s);
    std::string temp;

    while (std::getline(iss, temp, delim)) {
        sv.emplace_back(std::move(temp));
    }
}

//std::string UTF8ToGB(const char* str)
//{
//    std::string result;
//    WCHAR *strSrc;
//    LPSTR szRes;

//    //获得临时变量的大小
//    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
//    strSrc = new WCHAR[i+1];
//    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

//    //获得临时变量的大小
//    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
//    szRes = new CHAR[i+1];
//    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

//    result = szRes;
//    delete []strSrc;
//    delete []szRes;

//    return result;
//}