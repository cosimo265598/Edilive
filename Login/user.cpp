#include "user.h"

User::User(QString user, QString password) :
    user(user),
    password(password)
{
}

QString User::getUser(){
    return this->user;
}

QString User::getPassword(){
    return password;
}
