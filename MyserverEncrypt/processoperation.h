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
#include "taskfactory.h"

class ProcessOperation: public QObject
{
    Q_OBJECT
private:
    TypeOperation tipo;
    static ProcessOperation *instance;
    static std::once_flag    inited;

    ProcessOperation(QObject *parent);

public:
    static ProcessOperation *getInstance (QObject *parent) {
      std::call_once(inited, [&] () {
          instance = new ProcessOperation(parent);
        });
      return instance;
    }

    QString checkTypeOperationGranted(TypeOperation type);
    void process(TypeOperation message, QWebSocket* socket, QJsonObject& data);
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
