#ifndef USER_H
#define USER_H

#include <QObject>

class User
{
public:
    User(QString username, QString password);

private:
    QString username;
    QString password;

public:
    QString getUsername();
    QString getPassword();
};

#endif // USER_H
