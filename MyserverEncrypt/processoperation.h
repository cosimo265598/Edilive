#ifndef PROCESSOPERATION_H
#define PROCESSOPERATION_H

#include <QDataStream>
#include <QByteArray>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"

enum TypeOperation : quint16
{
    // Login messages
    LoginRequest,
    LoginChallenge,
    LoginUnlock,
    LoginGranted,
    LoginError,

    // Account messages
    AccountCreate,
    AccountUpdate,
    AccountConfirmed,
    AccountError,

    // Logout message
    Logout,
    // Others
    Failure
};

class ProcessOperation
{
private:
    TypeOperation tipo;

public:
    ProcessOperation(QWebSocketServer *s);
    QString typeOp(TypeOperation type);
    void process(TypeOperation message, QSslSocket* socket);
    ~ProcessOperation();

signals:

    ProcessOperation loginRequest(QWebSocket* clientSocket, QString username);
    ProcessOperation loginUnlock(QWebSocket* clientSocket, QByteArray token);

    ProcessOperation accountCreate(QWebSocket* lientSocket, QString username, QString nickname, QImage icon, QString password);
    ProcessOperation accountUpdate(QWebSocket* lientSocket, QString nickname, QImage icon, QString password);

};

#endif // PROCESSOPERATION_H
