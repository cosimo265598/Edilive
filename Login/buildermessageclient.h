#ifndef BUILDERMESSAGECLIENT_H
#define BUILDERMESSAGECLIENT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>
#include <QBuffer>

#include "updateuser.h"

class BuilderMessageClient
{
public:
    static void MessageSendToServer(QByteArray& byte,QJsonDocument jsonToSend);
    static void MessageSendToServer(QByteArray& byte,QByteArray& appendByte);


    static QJsonDocument MessageInsert(char car, std::vector<int> posf, QString id, QString siteid);
    static QJsonDocument MessageDelete(QString docURI, char car, std::vector<int> pos, QString id, QString siteid);

    static QJsonDocument MessageTest(QString data);
    static QJsonDocument MessageLogin(QString username);
    static QJsonDocument MessageLoginUnlock(QString salt,QString nonce, QString psw);
    static QJsonDocument MessageRegisterAccount(QString usr, QString passwd);

    // File management
    static QJsonDocument MessageOpenDir();
    static QJsonDocument MessageCreateNewFile(QString nomefile);
    static QJsonDocument MessageGenericErrorFile(QString data);
    static QJsonDocument MessageOpenFile(QString nomefile);
    static QJsonDocument MessagedDeleteFile(QString nomefile);

    // profile management
    static QJsonDocument MessageSubscriberInfoRequest();
    static QJsonDocument MessagedUpdateProfileRequest(QString nickname, QString password, QByteArray serializedImage);

    //TextEditor
    static QJsonDocument MessageRemoveClientWorkspace(QString fileName);
};

#endif // BUILDERMESSAGECLIENT_H
