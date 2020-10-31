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

#include "workspace.h"
#include "tasks.h"
#include "serverdatabase.h"
#include "serverexception.h"
#include "userdata.h"
#include "client.h"
#include "ui_mainwindow.h"


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
    QMap<QString, QSharedPointer<Workspace>>& workspaces;

    ProcessOperation(QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, QMap<QString, QSharedPointer<Workspace>>& workspaces);

    Tasks *createTask(QObject *parent,
                      QJsonObject request,
                      QWebSocket* socket,
                      QMap<QWebSocket*, QSharedPointer<Client>>& clients,
                      QMap<QString, UserData>& users,
                      QMap<QString, QSharedPointer<Workspace>>& workspaces,
                      TypeOperation typeOp,
                      Ui::MainWindow* ui);

public:
    static ProcessOperation *getInstance (QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, QMap<QString, QSharedPointer<Workspace>>& workspaces) {
      std::call_once(inited, [&] () {
          instance = new ProcessOperation(parent, clients, users, workspaces);
        });
      return instance;
    }

    void process(QWebSocket* socket, QJsonObject data,Ui::MainWindow* ui);
    ~ProcessOperation();

signals:
    void printUiServer(QString message);
public slots:
    void onLoginError(QWebSocket* socket, QString errorMessage);
    void onMessageChallenge(QWebSocket* socket, QString salt, QString challenge);
    void onMessageChallengePassed(QWebSocket* , QString);
    void onAccountCreationError(QWebSocket* , QString);
    void onAccountConfirmed(QWebSocket*, QString);
    void onOpenDirOfClient(QWebSocket*, QJsonArray, QString);
    void onAccountUpdateSuccess(QWebSocket*, QString);
    void onPersonalDataOfClient(QWebSocket* , QString, QString, QImage);
    void onFileCreationError(QWebSocket* , QString);
    void onFileDeletionError(QWebSocket* , QString);
    void onOpenFile(QWebSocket*, QString);
    void onSocketAbort(QWebSocket*);
    void onInsertionChar(QWebSocket *, Symbol);
    void onDeletionChar(QWebSocket *, Symbol);
    void onRemoveClientFromWorkspace(QWebSocket *, QString);
    void onAccountUpdateError(QWebSocket *, QString);
    void onRequestShareFile(QWebSocket *, QString, QString);

};

#endif // PROCESSOPERATION_H
