#ifndef UPDATEUSER_H
#define UPDATEUSER_H

#include <QObject>

class UpdateUser : public QObject
{
public:
    UpdateUser(QString username = nullptr, QString nickname = nullptr, QByteArray serializedImage = nullptr, QString password = nullptr);

    //Copy constructor
    UpdateUser(const UpdateUser &user2);

private:
    QString username;
    QString nickname;
    QByteArray serializedImage;
    QString password;

public:
    QString getUsername();
    QString getNickname();
    QString getPassword();
    QByteArray getSerializedImage();
    void setUsername(QString);
    void setNickname(QString);
    void setPassword(QString);
    void setSerializedImage(QByteArray);
    void reset();
};

#endif // UPDATEUSER_H
