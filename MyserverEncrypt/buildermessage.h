#ifndef BUILDERMESSAGE_H
#define BUILDERMESSAGE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QImage>


class BuilderMessage
{
public:
    static QJsonDocument MessageLogin();
    static QJsonDocument MessageChallege(QString salt, QString nonce);
    static QJsonDocument MessageChallegePassed(QString data);
    static QJsonDocument MessageLoginError(QString data);
    static QJsonDocument MessageAccountError(QString data);
    static QJsonDocument MessageAccountConfirmed(QString data);

    static QJsonDocument MessageOpenDirOfClient(QJsonArray listfiles);
    static QJsonDocument MessageFileClientError(QString error);
    static QJsonDocument MessageHeaderFile(QString data);

    static QJsonDocument MessageProfileData(QString username,QString nickname,QImage ico);
};


#endif // BUILDERMESSAGE_H
