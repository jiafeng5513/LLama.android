#include "TalkListModel.h"

#include <QTimer>
#include <QDateTime>
#include <QRandomGenerator>

#include "HTTPRequest.hpp"

#include "json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

using json = nlohmann::json;

TalkListModel::TalkListModel(QObject *parent)
        : QAbstractListModel(parent) {
    chatRequest_ = new ChatRequest(this);
    connect(chatRequest_, SIGNAL(requestReturn()), this, SLOT(onChatRequestFinish()));
    connect(chatRequest_, SIGNAL(newResponse(QString)), this, SLOT(onNewResponse(QString)));
}

int TalkListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return talkList.count();
}

QVariant TalkListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    const int row = index.row();
    auto item = talkList.at(row);
    switch (role) {
        default:
            break;
            //基本
        case Qt::UserRole + 0:
            return item->id;
        case Qt::UserRole + 1:
            return item->user;
        case Qt::UserRole + 2:
            return item->sender;
        case Qt::UserRole + 3:
            return QDateTime::fromMSecsSinceEpoch(item->datetime).toString("hh:mm:ss yyyy-MM-dd");
        case Qt::UserRole + 4:
            return item->type;
        case Qt::UserRole + 5:
            return item->status;
            //文本
        case Qt::UserRole + 100: {
            //没有判断转换有效性
            TalkDataText *talk_data = static_cast<TalkDataText *>(item.get());
            return talk_data->text;
        }
            //图片
            //音频
        case Qt::UserRole + 300: {
            TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
            return talk_data->path;
        }
        case Qt::UserRole + 301: {
            TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
            return talk_data->text;
        }
        case Qt::UserRole + 302: {
            TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
            return talk_data->duration;
        }
        case Qt::UserRole + 303: {
            TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
            return talk_data->durationStr();
        }
            //视频
    }
    return QVariant();
}

QHash<int, QByteArray> TalkListModel::roleNames() const {
    return QHash<int, QByteArray>{
            //基本信息
            {Qt::UserRole + 0,   "id"},
            {Qt::UserRole + 1,   "user"},
            {Qt::UserRole + 2,   "sender"},
            {Qt::UserRole + 3,   "datetime"},
            {Qt::UserRole + 4,   "type"},
            {Qt::UserRole + 5,   "status"}
            //文本
            ,
            {Qt::UserRole + 100, "text_text"}
            //图片
            //,{ Qt::UserRole+200, "image_path" }
            //语音
            ,
            {Qt::UserRole + 300, "audio_path"},
            {Qt::UserRole + 301, "audio_text"},
            {Qt::UserRole + 302, "audio_duration"},
            {Qt::UserRole + 303, "audio_durationStr"}
            //视频
            //,{ Qt::UserRole+400, "video_path" }
    };
}

void TalkListModel::clearModel() {
    beginResetModel();
    talkList.clear();
    endResetModel();
}

void TalkListModel::appendText(const QString &user,
                               const QString &sender,
                               const QString &text) {
    TalkDataText *talk_data = new TalkDataText;
    talk_data->id = 0; //暂时无用
    talk_data->user = user;
    talk_data->sender = sender;
    talk_data->datetime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    talk_data->type = TalkData::Text;
    talk_data->status = TalkData::ParseSuccess;
    talk_data->text = text + " "; // 防止ui上出现异常的换行

    beginInsertRows(QModelIndex(), talkList.count(), talkList.count());
    talkList.push_back(QSharedPointer<TalkDataBasic>(talk_data));
    endInsertRows();
}

void TalkListModel::appendAudio(const QString &user,
                                const QString &sender) {
    TalkDataAudio *talk_data = new TalkDataAudio;
    talk_data->id = 0; //暂时无用
    talk_data->user = user;
    talk_data->sender = sender;
    talk_data->datetime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    talk_data->type = TalkData::Audio;
    talk_data->status = TalkData::DataReady;
    //ms，显示前先把时长计算好，此处随机值测试语音块长度
    talk_data->duration = 1000 * QRandomGenerator::global()->bounded(1, 15);
    talk_data->path = "qrc:/Image/audio.wav";
    //talk_data->text="";

    beginInsertRows(QModelIndex(), talkList.count(), talkList.count());
    talkList.push_back(QSharedPointer<TalkDataBasic>(talk_data));
    endInsertRows();
}

void TalkListModel::sendPrompt(const QString &prompt) {
    // human send
    appendText("B", "B", prompt);

    this->chatRequest_->sendPrompt(prompt.toStdString()); // sed prompt to request obj
//    this->chatRequest_->start();
    this->chatRequest_->requestWithQt();
    // todo: set bot chat box to loading status
//    std::cout<<"TalkListModel::sendPrompt: start request"<<std::endl;
//    auto answer = QString::fromStdString(this->request(prompt));
//    std::cout<<"TalkListModel::sendPrompt: finish request"<<std::endl;
    // bot send
//    appendText("A","B", answer);
}

void TalkListModel::parseRow(int row) {
    //解析状态
    if (!isVaidRow(row))
        return;
    auto item = talkList.at(row);
    TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
    if (talk_data) {
        talk_data->status = TalkData::ParseOn;
        emit dataChanged(index(row, 0), index(row, 0));
    }

    //模拟解析结束
    QTimer::singleShot(1000, [this, row] {
        if (!isVaidRow(row))
            return;
        auto item = talkList.at(row);
        TalkDataAudio *talk_data = static_cast<TalkDataAudio *>(item.get());
        if (talk_data) {
            talk_data->text = (row % 2) ? "解析完成(This is dummy message)! " : "";
            talk_data->status = TalkData::ParseSuccess;
            emit dataChanged(index(row, 0), index(row, 0));
        }
    });
}

bool TalkListModel::isVaidRow(int row) const {
    return (row >= 0 && row < talkList.count());
}



void TalkListModel::appTextToLastBubbles(const QString &msg) {
    beginResetModel();
    auto item = talkList.back();
    reinterpret_cast<TalkDataText *>(item.get())->text += msg;
    endResetModel();
}
/// 响应newResponse信号，如果是第一个token,则新建话泡，否则则向最后一个话泡进行追加
/// \param msg
void TalkListModel::onNewResponse(QString msg) {
    if (responseStatus_ == RESPONSE_STATUS::FIRST_TOKEN){
        appendText("A", "B", msg);
        responseStatus_ = RESPONSE_STATUS::STREAMING;
        std::cout<< "first : "<<msg.toStdString()<<std::endl;
    } else{
        appTextToLastBubbles(msg);
        std::cout<< "append : "<<msg.toStdString()<<std::endl;
    }
}
/// 响应requestReturn信号，翻转状态。注意，此处不需要维护prompt json, prompt json的历史记录由request类维护。
void TalkListModel::onChatRequestFinish() {
    responseStatus_ = RESPONSE_STATUS::FIRST_TOKEN;
//    auto answer = this->chatRequest_->getAnswer();
//    appendText("A", "B", QString::fromStdString(answer));
    // todo：计算生成速度

}