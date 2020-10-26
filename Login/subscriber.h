#ifndef SUBSCRIBER_H
#define SUBSCRIBER_H

#include <QObject>

class Subscriber
{
public:
    Subscriber(QString username, QString nickname, QByteArray serializedImage);

private:
    QString username;
    QString nickname;
    QByteArray serializedImage;

public:
    QString& getUsername();
    QString& getNickname();
    QByteArray& getSerializedImage();
};

#endif // SUBSCRIBER_H
