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
    QMap<QWebSocket*, QSharedPointer<Client>>& clients;
    QMap<QString, UserData>& users;

    ProcessOperation(QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users);

    Tasks *createTask(QObject *parent, QJsonObject request, QWebSocket* socket, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation typeOp);

public:
    static ProcessOperation *getInstance (QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users) {
      std::call_once(inited, [&] () {
          instance = new ProcessOperation(parent, clients, users);
        });
      return instance;
    }

    //QString checkTypeOperationGranted(TypeOperation type);
    void process(QWebSocket* socket, QJsonObject data);
    ~ProcessOperation();

signals:

public slots:
    void onLoginError(QWebSocket* socket, QString errorMessage);
    void onMessageChallenge(QWebSocket* socket, QString salt, QString challenge);
    void onMessageChallengePassed(QWebSocket* , QString);
    void onAccountCreationError(QWebSocket* , QString);
    void onAccountConfirmed(QWebSocket*, QString);
    void onOpenDirOfClient(QWebSocket*, QJsonArray);
    void onAccountUpdateSuccess(QWebSocket*, QString);
    void onPersonalDataOfClient(QWebSocket* , QString, QString, QImage);
    void onFileCreationError(QWebSocket* , QString);
    void onFileDeletionError(QWebSocket* , QString);
    void onOpenFile(QWebSocket* , QString, QByteArray);
};

#endif // PROCESSOPERATION_H
