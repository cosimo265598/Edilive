#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QProcess>
#include <textedit.h>
#include <QDesktopWidget>
#include <QScreen>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QTimer>
#include <textedit.h>
#include <QAbstractSocket>
#include <QChar>


#include "mainwindow.h"
#include "ui_mainwindowstacked.h"
#include "user.h"
#include "homepage.h"
#include "logindialog.h"
#include "buildermessageclient.h"
#include "startupstackeddialog.h"
#include "connectionwaitingdialog.h"
#include "mainwindowstacked.h"
#include "subscriber.h"
#include "SharedFile.h"

enum ClientStatus : quint32
{
    Startup,
    LoginRequest,
    Connected,
    RegistrationRequest,
    Disconnected,
    ReConnect
};

struct loginUser_t{
    QString username;
    QString password;
};

struct subscriber_t{
    QString username;
    QString nickname;
    QByteArray serializedImage;
};

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

private slots:

    void onLoginRequest(QString username, QString password);
    void onRegistrationRequest(QString username, QString password);
    void onConnected();
    void onSslErrors(const QList<QSslError> &errors);
    void MessageReceivedFromServer(const QByteArray& message);
    void onDisconnection();
    void onConnectionSuccess();
    void onConnectionFailure();
    void onFileHandlerDbClicked(QString fileName);
    void onCreateNewFileRequest(QString fileName);
    void onDeleteFileRequest(QString fileName);
    void onUpdateProfileRequest(updateUser_t);

public slots:
    void ping();
    void errorSocket(QAbstractSocket::SocketError error);
    void closeControll();
    void localInsertion(QString c, int pos);
    void onRemoveClientFromWorkspace(QString fileName);

private:
    MainWindowStacked *mainWindowStacked=nullptr;
    StartupStackedDialog *stackedDialog=nullptr;
    HomePage *homePage;
    TextEdit *textEditor;
    QSharedPointer<QWebSocket> m_webSocket;
    QString urlForConnection;
    QTimer *waitingTimer;
    qint32 reconnectionRetries;
    quint32 clientStatus;
    quint32 old_clientstatus;
    subscriber_t subscriber;
    ConnectionWaitingDialog waitingDialog;
    loginUser_t user;
    updateUser_t updateUser;
    SharedFile *sf;
    QList<subscriber_t> listUserOnWorkspace;


    void resetUser();
    void resetUpdateUser();
    QByteArray saveAccountImage(QString serializedImage);
    void createMainWindowStacked();
    void startTextEditor(QString fileName);
    void subscriberInfoRequest();
    void fileHandlersRequest();
    void standardInsert(QJsonObject);

signals:
    void registrationFailure(QString errorMessage);
    void loginFailure(QString errorMessage);
    void receivedFileHandlers(QJsonArray);
    void loadSubscriberInfo(QString, QString, QByteArray);
    void newFileCreationFailure(QString errorMessage);
    void updateSuccess();
    void refreshText(QString);
    void updateListUsersConnected(int id,QString username,QImage img);
};
#endif // CLIENT_H
