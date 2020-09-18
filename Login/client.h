#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
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
#include <clientfilesystem.h>
#include <textedit.h>
#include "user.h"
#include "homepage.h"
#include "logindialog.h"

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

private slots:
    void on_sign_clicked();
    void fieldForSign(bool state);

    void on_back_clicked();

    void onLoginRequest(QString user, QString password);
    void onConnected();
    void onSslErrors(const QList<QSslError> &errors);
    void on_sign_account_clicked();
    void MessageReceivedFromServer(const QByteArray& message);
    void Registeruser();
    void StartNewWindows();
    void StartEditorText(QString fileeditor);
    void onDisconnection();
    void onConnectionSuccess();
    void onConnectionFailure();

private:
    LoginDialog *loginDialog;
    HomePage *homePage;
    TextEdit *editor;
    QSharedPointer<QWebSocket> m_webSocket;
    QString urlForConnection;
    QTimer *reconnectionTimer;
    qint32 reconnectionRetries;
    User *user;
};
#endif // CLIENT_H
