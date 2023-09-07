//
// Created by anna on 23-8-22.
//

#ifndef CHATBOT_CHATREQUEST_H
#define CHATBOT_CHATREQUEST_H

#include <QThread>
#include "json.hpp"
#include <QJsonObject>

class ChatRequest : public QThread {
    Q_OBJECT
public:
    ChatRequest(QObject *parent = nullptr);
    ~ChatRequest() override;

    void run() override;

    void sendPrompt(const std::string &prompt);
    std::string getAnswer();
    void requestWithQt();
private:
    QString answer_;
    std::vector<std::string> prompts_;
    // todo: other params need to pass to server, need a delegate var here and getters/setters.
    nlohmann::json requestPayload_;
    QJsonObject requestPayloadForQt_;

    void split(const std::string& s, std::vector<std::string>& sv, const char delim = ' ');
    void requestWithHttp();

signals:
    void requestReturn();
    void newResponse(QString msg);
};


#endif //CHATBOT_CHATREQUEST_H
