#ifndef USER_H
#define USER_H
#include <QString>
#include <QDataStream>

class User
{
    friend QDataStream &operator<<(QDataStream &, const User &);
    friend QDataStream &operator>>(QDataStream &, User &);

private:
    int age;
    QString nome;
    QString cognome;

public:
    User();
    User(QString n,QString c,int a);
    int getAge();
    QString getNome();
    QString getCognome();
    QString print();

};

#endif // USER_H
