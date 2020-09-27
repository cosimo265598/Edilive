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
#include "mainwindowstacked.h"

enum ClientStatus : quint32
{

    LoginRequest,
    LoggedIn,
    RegistrationRequest
};

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

private slots:

    void onLoginRequest(QString user, QString password);
    void onConnected();
    void onSslErrors(const QList<QSslError> &errors);
    void on_sign_account_clicked();
    void MessageReceivedFromServer(const QByteArray& message);
    void Registeruser();
    void createMainWindowStacked();
    void StartEditorText(QString fileeditor);
    void onDisconnection();
    void onConnectionSuccess();
    void onConnectionFailure();

private:
    MainWindowStacked *mainWindowStacked;
    StartupStackedDialog *stackedDialog;
    HomePage *homePage;
    TextEdit *editor;
    QSharedPointer<QWebSocket> m_webSocket;
    QString urlForConnection;
    QTimer *reconnectionTimer;
    qint32 reconnectionRetries;
    User *user;
    quint32 clientStatus;

signals:
    void loginFailure();
    void loadFileHandlers(QJsonArray);
};
#endif // CLIENT_H
