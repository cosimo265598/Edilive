#ifndef PROCESSOPERATION_H
#define PROCESSOPERATION_H

#include <QDataStream>
#include <QByteArray>
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QImage>
#include <mutex>
#include <iostream>
#include <QRunnable>
#include <QThreadPool>

#include "tasks.h"
#include "serverdatabase.h"
#include "serverexception.h"
#include "userdata.h"
#include "client.h"


class ProcessOperation: public QObject
{
    Q_OBJECT
private:
    TypeOperation tipo;
    static ProcessOperation *instance;
    static std::once_flag    inited;
    //Tasks *tasks;
    QMap<QWebSocket*, QSharedPointer<Client>> clients;
    QMap<QString, UserData> users;
    ServerDatabase database;

    ProcessOperation(QObject *parent, ServerDatabase& database, QMap<QWebSocket*, QSharedPointer<Client>> clients, QMap<QString, UserData>& users);
    void serverAccountCreate(QWebSocket *clientSocket, QJsonObject request);

public:
    static ProcessOperation *getInstance (QObject *parent, ServerDatabase& database, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users) {
      std::call_once(inited, [&] () {
          instance = new ProcessOperation(parent, database, clients, users);
        });
      return instance;
    }

    QString checkTypeOperationGranted(TypeOperation type);
    QRunnable *process(TypeOperation message, QWebSocket* socket, QJsonObject data);
    ~ProcessOperation();

signals:

    void  loginRequest(QWebSocket* clientSocket, QString username);
    void  loginUnlock(QWebSocket* clientSocket, QString token);

    void  accountCreate(QWebSocket* clientSocket, QString username, QString password);
    //void  accountUpdate(QWebSocket* clientSocket, QString nickname, QImage icon, QString password);
    void  SimpleMessage(QWebSocket* clientSocket, QString mess);

    /* FILE */
    void OpenDirOfClient(QWebSocket* clientSocket);
    void CreateFileForClient(QWebSocket* clientSocket, QString filename);
    void OpenFileForClient(QWebSocket* clientSocket, QString filename);
    void DeleteFileForClient(QWebSocket* clientSocket, QString filename);

    /* Profile */
    void PersonalDataOfClient(QWebSocket* clientSocket);
    void accountUpdate(QWebSocket*, QString, QString, QString);
};

#endif // PROCESSOPERATION_H
