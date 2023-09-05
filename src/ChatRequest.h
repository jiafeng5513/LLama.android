//
// Created by anna on 23-8-22.
//

#ifndef CHATBOT_CHATREQUEST_H
#define CHATBOT_CHATREQUEST_H

#include <QThread>
#include "json.hpp"

class ChatRequest : public QThread {
    Q_OBJECT
public:
    ChatRequest(QObject *parent = nullptr);
    ~ChatRequest() override;

    void run() override;

    void sendPrompt(const std::string &prompt);
    std::string getAnswer();
private:
    std::string answer_;
    std::vector<std::string> prompts_;
    // todo: other params need to pass to server, need a delegate var here and getters/setters.
    nlohmann::json requestPayload_;
    void split(const std::string& s, std::vector<std::string>& sv, const char delim = ' ');
signals:
    void requestReturn();
    void newResponse(QString msg);
};


#endif //CHATBOT_CHATREQUEST_H
