#ifndef PROCESSOPERATION_H
#define PROCESSOPERATION_H

#include <QDataStream>
#include <QByteArray>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QObject>


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

class ProcessOperation: public QObject
{
    Q_OBJECT
private:
    TypeOperation tipo;

public:
    ProcessOperation(QObject *parent );
    QString typeOp(TypeOperation type);
    void process(TypeOperation message, QWebSocket* socket);
    ~ProcessOperation();

signals:

    //void  loginRequest(QWebSocket* clientSocket, QString username);
    //void  loginUnlock(QWebSocket* clientSocket, QByteArray token);

    //void  accountCreate(QWebSocket* clientSocket, QString username, QString nickname, QImage icon, QString password);
    //void  accountUpdate(QWebSocket* clientSocket, QString nickname, QImage icon, QString password);
    void  SimpleMessage(QWebSocket* clientSocket, QString mess);

};

#endif // PROCESSOPERATION_H
