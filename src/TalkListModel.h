#ifndef TALKLISTMODEL_H
#define TALKLISTMODEL_H

#include <QAbstractListModel>
#include "TalkListDefine.h"

#include "ChatRequest.h"

//聊天框ListView的model
class TalkListModel : public QAbstractListModel {
Q_OBJECT

public:
    explicit TalkListModel(QObject *parent = nullptr);

    //data
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    //清空数据
    Q_INVOKABLE void clearModel();
    //测试发送文本
    Q_INVOKABLE void appendText(const QString &user, const QString &sender, const QString &text);
    //测试发送语音
    Q_INVOKABLE void appendAudio(const QString &user, const QString &sender);

    Q_INVOKABLE void sendPrompt(const QString &prompt);

    //解析，如语音转文字，文档转换等
    Q_INVOKABLE void parseRow(int row);

private:
    enum RESPONSE_STATUS {
        FIRST_TOKEN,
        STREAMING,
    };

    bool isVaidRow(int row) const;

    void appTextToLastBubbles(const QString &msg);

    ChatRequest *chatRequest_;
    RESPONSE_STATUS responseStatus_ = RESPONSE_STATUS::FIRST_TOKEN;
private:
    //会话数据
    QList<QSharedPointer<TalkDataBasic>> talkList;
public slots:

    void onNewResponse(QString msg);

    void onChatRequestFinish();
};

#endif // TALKLISTMODEL_H
