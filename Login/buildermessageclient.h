#ifndef BUILDERMESSAGECLIENT_H
#define BUILDERMESSAGECLIENT_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>

class BuilderMessageClient
{
public:
    static QJsonDocument MessageTest(QString data);
    static QJsonDocument MessageLogin(QString username);
    static QJsonDocument MessageLoginUnlock(QString salt,QString nonce, QString psw);
    static QJsonDocument MessageRegisterAccount(QString usr, QString passwd, QString nick, QImage img);

    // File management
    static QJsonDocument MessageOpenDir();
    static QJsonDocument MessageCreateNewFile(QString nomefile);
    static QJsonDocument MessageGenericErrorFile(QString data);
};

#endif // BUILDERMESSAGECLIENT_H
