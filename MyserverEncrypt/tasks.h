#ifndef TASKS_H
#define TASKS_H

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QJsonObject>
#include <QSqlDatabase>

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
    DeleteFile,
    // add other enum below, and do not change the order of list
    UnknowMessage
};


class Tasks : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Tasks(QObject *parent,
          QJsonObject request, QWebSocket* socket, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation type );

    void run() override;
    ~Tasks();

private:
    QMap<QWebSocket*, QSharedPointer<Client>>& clients;
    QMap<QString, UserData>& users;
    TypeOperation typeOp;
    QJsonObject request;
    QWebSocket* socket;
    ServerDatabase database;
    QString threadId;

    void serverAccountCreate(QJsonObject request);
    void serverLoginRequest();
    void serverLoginUnlock();
    void serverAccountCreate();
    void serverOpenDirOfClient();
    void serverUpdateProfileClient();
    void serverPersonalDataOfClient();
    void serverCreateFileForClient();
    void serverDeleteFileForClient();
    void serverOpenFile();

signals:
    void loginError(QWebSocket*,QString);
    void messageChallenge(QWebSocket*, QString, QString);
    void messageChallegePassed(QWebSocket*, QString);
    void accountCreationError(QWebSocket*, QString);
    void accountConfirmed(QWebSocket*, QString);
    void openDirOfClient(QWebSocket*, QJsonArray);
    void updateProfileClient(QWebSocket*, QJsonArray);
    void accountUpdateSuccess(QWebSocket*, QString);
    void personalDataOfClient(QWebSocket*, QString, QString, QImage);
    void fileCreationError(QWebSocket*, QString);
    void fileDeletionError(QWebSocket*, QString);
    void openFile(QWebSocket*, QString, QByteArray);

};

#endif // TASKS_H
