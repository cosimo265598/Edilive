#ifndef TASKS_H
#define TASKS_H

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QJsonObject>

#include "serverdatabase.h"
#include "serverexception.h"
#include "userdata.h"
#include "client.h"
#include "buildermessage.h"
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

    // File Management
    OpenDirectory,
    OpenFile,
    CreateFile,
    ErrorFile,
    // Logout message
    Logout,
    // Others
    Failure,

    // personal image
    ProfileData,
    DeleteFile
    // add other enum below, and do not change the order of list

};


class Tasks : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Tasks(QObject *parent, QWebSocket *clientSocket,
          QJsonObject request, ServerDatabase* database, QMap<QWebSocket*, QSharedPointer<Client>>* clients, QMap<QString, UserData>* users, TypeOperation type );
    void serverAccountCreate(QWebSocket *socket, QJsonObject request);
    void test(QJsonObject m);
    void run() override;

private:
    QMap<QWebSocket*, QSharedPointer<Client>> *clients;
    QMap<QString, UserData>* users;
    ServerDatabase* database;
    TypeOperation type;
    QWebSocket *clientSocket;
    QJsonObject request;

signals:

};

#endif // TASKS_H
