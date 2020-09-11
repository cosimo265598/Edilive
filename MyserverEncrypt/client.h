#ifndef CLIENT_H
#define CLIENT_H


#include "userdata.h"
#include <QSslSocket>
#include "QtWebSockets/QWebSocket"


class Client
{

private:

    QWebSocket* socket;
    UserData* activeUser;
    bool logged;

    QByteArray nonce;
    static const QString nonceCharacters;

public:

    Client(QWebSocket* s);

    ~Client();

    void login(UserData* user);
    void logout();
    bool isLogged();

    bool authenticate(QByteArray token);
    QByteArray challenge(UserData* user);

    /* getters */
    UserData* getUser() const;
    int getUserId() const;
    QString getUsername() const;
    QWebSocket* getSocket() const;
    qintptr getSocketDescriptor() const;
};



#endif // CLIENT_H
