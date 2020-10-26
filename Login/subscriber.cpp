#include "subscriber.h"

Subscriber::Subscriber(QString username, QString nickname, QByteArray serializedImage):
    username(username),
    nickname(nickname),
    serializedImage(serializedImage)
{
}

QString& Subscriber::getUsername(){
    return this->username;
}

QString& Subscriber::getNickname(){
    return this->nickname;
}

QByteArray& Subscriber::getSerializedImage(){
    return this->serializedImage;
}
