#include "user.h"

User::User(QString username, QString password) :
    username(username),
    password(password)
{
}

QString User::getUsername(){
    return this->username;
}

QString User::getPassword(){
    return password;
}
