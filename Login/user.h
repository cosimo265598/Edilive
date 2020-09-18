#ifndef USER_H
#define USER_H

#include <QObject>

class User
{
public:
    User(QString user, QString password);

private:
    QString user;
    QString password;

public:
    QString getUser();
    QString getPassword();

signals:

};

#endif // USER_H
