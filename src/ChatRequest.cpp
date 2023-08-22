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

}

ChatRequest::~ChatRequest() {

}

void ChatRequest::run() {
    // todo: creat prompt.json with this->prompts

    // 以utf8读取文件，直接构造post请求，返回的response使用GBK解码显示
    try
    {
        http::Request request{"http://127.0.0.1:8080/completion"};
//
        std::ifstream f("/home/anna/WorkSpace/celadon/LLama.android/prompt.json");
        json data = json::parse(f);

        std::stringstream sss;
        sss << data;

//        const std::string body = UTF8ToGB(sss.str().c_str());
        const std::string body = sss.str();
        const auto response = request.send("POST", body, {{"Content-Type", "application/json"}});
        auto results = std::string{response.body.begin(), response.body.end()};

        std::vector<std::string> sv;
        split(results, sv, '\n');
        std::stringstream ssr;
//        std::regex word_regex("content: (\\w+),");
        for (int i =0; i<sv.size();i++){
            if (not sv[i].empty()){
                auto payload = sv[i].substr(6, sv[i].length()-6);  //

                json temp = json::parse(payload);
                auto content = temp["content"].get<std::string>();
//                auto content_gbk = UTF8ToGB(content.c_str());
                ssr <<content;
            }
        }
        this->answer_ = ssr.str();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }

    emit requestReturn();
}

void ChatRequest::sendPrompt(const std::string &prompt) {

}

std::string ChatRequest::getAnswer() {
    return this->answer_;
}

void ChatRequest::split(const std::string& s, std::vector<std::string>& sv, const char delim) {
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