#ifndef PROCESSOPERATION_H
#define PROCESSOPERATION_H

#include <QDataStream>
#include <QByteArray>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"

enum TypeOperation : quint16
{
    // Tests
    Simplemessage,
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
    ProcessOperation();
    ProcessOperation(QWebSocketServer *s);
    QString typeOp(TypeOperation type);
    void process(TypeOperation message, QWebSocket* socket);
    ~ProcessOperation();

signals:

    ProcessOperation loginRequest(QWebSocket* clientSocket, QString username);
    ProcessOperation loginUnlock(QWebSocket* clientSocket, QByteArray token);

    ProcessOperation accountCreate(QWebSocket* clientSocket, QString username, QString nickname, QImage icon, QString password);
    ProcessOperation accountUpdate(QWebSocket* clientSocket, QString nickname, QImage icon, QString password);
    ProcessOperation SimpleMessage(QWebSocket* clientSocket, QString mess);

};

#endif // PROCESSOPERATION_H
