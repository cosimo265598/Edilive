#ifndef BUILDERMESSAGE_H
#define BUILDERMESSAGE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebSocket>
#include <QImage>

#include "Symbol.h"
#include "SharedFile.h"
#include "client.h"

#define KByte *1024
#define MByte *1024*1024
#define GByte *1024*1024*1024

class BuilderMessage
{
public:
    static void MessageSendToClient(QByteArray& byte,QJsonDocument jsonToSend);
    static void MessageSendToClient(QByteArray& byte,QByteArray& appendByte);
    static void MessageSendToClient(QByteArray& byte,SharedFile *newfile);

    static QJsonDocument MessageInsert(char car, std::vector<int> posf, QString id, QString siteid/*, QString iniziale*/);
    static QJsonDocument MessageConflictInsert(char car, std::vector<int>  newposf, std::vector<int>  oldposf, QString id, QString siteid);
    static QJsonDocument MessageDelete(char car, std::vector<int> posf, QString id, QString siteid);

    static QJsonDocument MessageLogin();
    static QJsonDocument MessageChallege(QString salt, QString nonce);
    static QJsonDocument MessageChallegePassed(QString data);
    static QJsonDocument MessageLoginError(QString data);
    static QJsonDocument MessageAccountError(QString data);
    static QJsonDocument MessageAccountConfirmed(QString data);

    static QJsonDocument MessageOpenDirOfClient(QJsonArray listfiles);
    static QJsonDocument MessageFileCreationError(QString error);
    static QJsonDocument MessageHeaderFile(QString data, QString creator, QList<QSharedPointer<Client>> connected);
    static QJsonDocument MessageFileDeletionError(QString error);
    static QJsonDocument MessageFileCreationSuccess(QString username,QString nickname,QImage icon);//

    static QJsonDocument MessageAccountInfo(QString username,QString nickname,QImage icon);
    static QJsonDocument MessageAccountUpdateSuccess(QString);
    static QJsonDocument MessageInsertClientWorkspace(QString username, QString nickname, QImage ico);
};


#endif // BUILDERMESSAGE_H
