//
// Created by anna on 23-8-22.
//

#include "ChatRequest.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonArray>

#include "json.hpp"
#include "HTTPRequest.hpp"

using json = nlohmann::json;

ChatRequest::ChatRequest(QObject *parent) {
    requestPayload_["prompt"] = "";
    requestPayload_["temperature"] = 0.2;
    requestPayload_["top_k"] = 40;
    requestPayload_["top_p"] = 0.9;
    requestPayload_["n_keep"] = 36;
    requestPayload_["n_predict"] = 2048;
    requestPayload_["stop"] = std::vector<std::string>({"### Human:"});
    requestPayload_["stream"] = true;

    requestPayloadForQt_["prompt"] = "";
    requestPayloadForQt_["temperature"] = 0.2;
    requestPayloadForQt_["top_k"] = 40;
    requestPayloadForQt_["top_p"] = 0.9;
    requestPayloadForQt_["n_keep"] = 36;
    requestPayloadForQt_["n_predict"] = 2048;
    QJsonArray arr;
    arr.append("### Human:");
    requestPayloadForQt_["stop"] = arr;
    requestPayloadForQt_["stream"] = true;
}

ChatRequest::~ChatRequest() {

}




void ChatRequest::run() {

}

void ChatRequest::sendPrompt(const std::string &prompt) {
    requestPayload_["prompt"] = requestPayload_["prompt"].get<std::string>() + " ### Human: " + prompt + " ### Assistant: ";
    requestPayloadForQt_["prompt"] = requestPayloadForQt_["prompt"].toString() + " ### Human: " + QString::fromStdString(prompt) + " ### Assistant: ";
}

std::string ChatRequest::getAnswer() {
    return this->answer_.toStdString();
}

void ChatRequest::split(const std::string &s, std::vector<std::string> &sv, const char delim) {
    sv.clear();
    std::istringstream iss(s);
    std::string temp;

    while (std::getline(iss, temp, delim)) {
        sv.emplace_back(std::move(temp));
    }
}

void ChatRequest::requestWithHttp() {
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
        this->answer_ = QString::fromStdString(ssr.str());
        // todo: 给payload 记录上下文
        requestPayload_["prompt"] = requestPayload_["prompt"].get<std::string>() + this->answer_.toStdString();
    }
    catch (const std::exception &e) {
        std::cerr << "Request failed, error: " << e.what() << '\n';
    }

    emit requestReturn();
}

void ChatRequest::requestWithQt() {
    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    const QUrl url(QStringLiteral("http://127.0.0.1:8080/completion"));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument doc(requestPayloadForQt_);
    QByteArray data = doc.toJson();
    std::cout<<data.toStdString()<<std::endl;
    QNetworkReply *reply = mgr->post(request, data);

    QObject::connect(reply, &QNetworkReply::readyRead, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString contents = QString::fromUtf8(reply->readAll());
            QString payload =contents.replace("data: ","");
            QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
            QString content = doc.object()["content"].toString();
            emit newResponse(content);
            answer_.append(content);
        }
        else{
            QString err = reply->errorString();
            qDebug() << err;
        }
    });

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        reply->deleteLater();
        requestPayloadForQt_["prompt"] = requestPayloadForQt_["prompt"].toString() + this->answer_;
        this->answer_.clear();
        emit requestReturn();
    });
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