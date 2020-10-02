#include "updateuser.h"

UpdateUser::UpdateUser(QString username, QString nickname, QByteArray serializedImage, QString password):
    username(username),
    nickname(nickname),
    serializedImage(serializedImage),
    password(password)
{
}

 UpdateUser::UpdateUser(const UpdateUser &user2){
     nickname = user2.nickname;
     serializedImage = user2.serializedImage;
     password = user2.password;
 }

QString UpdateUser::getUsername(){
    return this->username;
}

QString UpdateUser::getNickname(){
    return this->nickname;
}

QByteArray UpdateUser::getSerializedImage(){
    return this->serializedImage;
}

QString UpdateUser::getPassword(){
    return this->password;
}

void UpdateUser::setUsername(QString username){
    this->username = username;
}

void UpdateUser::setNickname(QString nickname){
    this->nickname = nickname;
}

void UpdateUser::setSerializedImage(QByteArray serializedImage){
    this->serializedImage = serializedImage;
}

void UpdateUser::setPassword(QString password){
    this->password = password;
}

void UpdateUser::reset(){
    this->password = nullptr;
    this->nickname = nullptr;
    this->serializedImage = nullptr;
}
