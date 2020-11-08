#ifndef BUILDERMESSAGECLIENT_H
#define BUILDERMESSAGECLIENT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>
#include <QBuffer>
#include <QJsonArray>
#include <QTextCodec>

#include "updateuser.h"

class BuilderMessageClient
{
public:
    static void MessageSendToServer(QByteArray& byte,QJsonDocument jsonToSend);
    static void MessageSendToServer(QByteArray& byte,QByteArray& appendByte);


    static QJsonDocument MessageInsert(QChar car, std::vector<int> posf, QString id, QString siteid);
    static QJsonDocument MessageDelete(QString URI, QChar car, std::vector<int> pos, QString id, QString siteid);

    static QJsonDocument MessageTest(QString data);
    static QJsonDocument MessageLogin(QString username);
    static QJsonDocument MessageLoginUnlock(QString salt,QString nonce, QString psw);
    static QJsonDocument MessageRegisterAccount(QString usr, QString passwd);

    // File management
    static QJsonDocument MessageOpenDir();
    static QJsonDocument MessageCreateNewFile(QString fileName);
    static QJsonDocument MessageGenericErrorFile(QString data);
    static QJsonDocument MessageOpenFile(QString URI, QString fileName);
    static QJsonDocument MessagedDeleteFile(QString URI, QString fileName);

    // profile management
    static QJsonDocument MessageSubscriberInfoRequest();
    static QJsonDocument MessagedUpdateProfileRequest(QString nickname, QString password, QByteArray serializedImage);

    //TextEditor
    static QJsonDocument MessageRemoveClientWorkspace(QString URI);
    static QJsonDocument MessagedCloseEditor(QString URI);
    static QJsonDocument MessageShareFile(QString username, QString URI);
};

#endif // BUILDERMESSAGECLIENT_H
