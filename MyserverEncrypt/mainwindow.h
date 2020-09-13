#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QNetworkSession>
#include <QLabel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDialog>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QtNetwork/QSslError>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QDateTime>
#include <QTextCursor>
#include <userdata.h>



#include "client.h"
#include "serverdatabase.h"
#include "processoperation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void SSLconfiguration(QSslConfiguration& conf);
private slots:

    void on_startserver_clicked();
    void on_stopserver_clicked();
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
    void onSslErrors(const QList<QSslError> &errors);
    void on_startserver_2_clicked();

public slots:
    void prepareToStart();

    void SimpleTextMessageTest();
    void serverLoginRequest(QWebSocket* clientSocket, QString username);
    void serverLoginUnlock(QWebSocket* clientSocket, QString token);
    void serverAccountCreate(QWebSocket* clientSocket, QString username, QString nickname, QImage icon, QString password);
    void serverErrorConnection(QWebSocketProtocol::CloseCode closeCode);

    /* FILE */
    void OpenDirOfClient(QWebSocket* clientSocket);
    void CreateFileForClient(QWebSocket* clientSocket, QString file);
    void OpenFileForClient(QWebSocket* clientSocket, QString file);

    /* Profile data */
    void PersonalDataOfClient (QWebSocket* clientSocket);

private:
    QSharedPointer<Ui::MainWindow> ui;
    QSharedPointer<QWebSocketServer> m_pWebSocketServer;
   // QList<QWebSocket *> m_clients;
    ServerDatabase database;
    QSslConfiguration sslconfig;
    QMap<QWebSocket*, QSharedPointer<Client>> clients;
    QMap<QString, UserData> users;
    int userId;


};
#endif // MAINWINDOW_H
