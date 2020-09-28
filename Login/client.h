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


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "user.h"
#include "homepage.h"
#include "logindialog.h"
#include "buildermessageclient.h"
#include "startupstackeddialog.h"
#include "connectionwaitingdialog.h"
#include "mainwindowstacked.h"
#include "subscriber.h"

enum ClientStatus : quint32
{
    Startup,
    LoginRequest,
    Connected,
    RegistrationRequest,
    Disconnected
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

private:
    MainWindowStacked *mainWindowStacked;
    StartupStackedDialog *stackedDialog;
    HomePage *homePage;
    TextEdit *textEditor;
    QSharedPointer<QWebSocket> m_webSocket;
    QString urlForConnection;
    QTimer *waitingTimer;
    qint32 reconnectionRetries;
    User *user;
    quint32 clientStatus;
    Subscriber *subscriber;
    ConnectionWaitingDialog waitingDialog;

    QByteArray saveAccountImage(QString serializedImage);
    void createMainWindowStacked();
    void startTextEditor(QString fileName);
    void subscriberInfoRequest();
    void fileHandlersRequest();

signals:
    void registrationFailure(QString errorMessage);
    void loginFailure(QString errorMessage);
    void receivedFileHandlers(QJsonArray);
    void loadSubscriberInfo(QString username, QString nickname, QByteArray);
    void newFileCreationFailure(QString errorMessage);
};
#endif // CLIENT_H
