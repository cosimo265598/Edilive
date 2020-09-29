#ifndef BUILDERMESSAGE_H
#define BUILDERMESSAGE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>


class BuilderMessage
{
public:
    static void MessageSendToClient(QByteArray& byte,QJsonDocument jsonToSend);
    static void MessageSendToClient(QByteArray& byte,QByteArray& appendByte);

    static QJsonDocument MessageLogin();
    static QJsonDocument MessageChallege(QString salt, QString nonce);
    static QJsonDocument MessageChallegePassed(QString data);
    static QJsonDocument MessageLoginError(QString data);
    static QJsonDocument MessageAccountError(QString data);
    static QJsonDocument MessageAccountConfirmed(QString data);

    static QJsonDocument MessageOpenDirOfClient(QJsonArray listfiles);
    static QJsonDocument MessageFileCreationError(QString error);
    static QJsonDocument MessageHeaderFile(QString data);
    static QJsonDocument MessageFileDeletionError(QString error);

    static QJsonDocument MessageAccountInfo(QString username,QString nickname,QImage icon);
};


#endif // BUILDERMESSAGE_H
