#include "mainwindow.h"
#include "processoperation.h"
#include <iostream>
#include <QObject>
#include <memory>

ProcessOperation* ProcessOperation::instance;
std::once_flag    ProcessOperation::inited;

ProcessOperation::ProcessOperation(QObject *parent, QMap<QWebSocket*,
QSharedPointer<Client>>& clients, QMap<QString, UserData>& users):QObject(parent),
    clients(clients),
    users(users)
{
}

Tasks *ProcessOperation::createTask(QObject *parent,
QJsonObject request, QWebSocket* socket, QMap<QWebSocket*,
QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation typeOp,
Ui::MainWindow* ui)
{
    Tasks *task = new Tasks(nullptr, request, socket, clients, users, typeOp,ui);

    connect(task,&Tasks::loginError, this, &ProcessOperation::onLoginError,Qt::DirectConnection);
    connect(task,&Tasks::messageChallenge, this, &ProcessOperation::onMessageChallenge,Qt::DirectConnection);
    connect(task,&Tasks::messageChallegePassed, this, &ProcessOperation::onMessageChallengePassed,Qt::DirectConnection);
    connect(task,&Tasks::accountCreationError, this, &ProcessOperation::onAccountCreationError,Qt::DirectConnection);
    connect(task,&Tasks::accountConfirmed, this, &ProcessOperation::onAccountConfirmed,Qt::DirectConnection);
    connect(task,&Tasks::openDirOfClient, this, &ProcessOperation::onOpenDirOfClient,Qt::DirectConnection);
    connect(task,&Tasks::personalDataOfClient, this, &ProcessOperation::onPersonalDataOfClient,Qt::DirectConnection);
    connect(task,&Tasks::accountUpdateSuccess, this, &ProcessOperation::onAccountUpdateSuccess,Qt::DirectConnection);
    connect(task,&Tasks::fileCreationError, this, &ProcessOperation::onFileCreationError,Qt::DirectConnection);
    connect(task,&Tasks::fileDeletionError, this, &ProcessOperation::onFileDeletionError,Qt::DirectConnection);
    connect(task,&Tasks::openFile, this, &ProcessOperation::onOpenFile,Qt::DirectConnection);

    connect(task,&Tasks::socketAbort, this, &ProcessOperation::onSocketAbort,Qt::QueuedConnection);
    connect(task,&Tasks::printUiServer,dynamic_cast<MainWindow*>(this->parent()),&MainWindow::printUiServer,Qt::QueuedConnection);
    connect(this,&ProcessOperation::printUiServer,dynamic_cast<MainWindow*>(this->parent()),&MainWindow::printUiServer,Qt::QueuedConnection);


    return task;
}


void ProcessOperation::process(QWebSocket* socket, QJsonObject request, Ui::MainWindow* ui)
{
    TypeOperation typeOp = (TypeOperation)(request["type"].toInt());

    Tasks *task = this->createTask(nullptr, request, socket, clients, users, typeOp,ui);

    QThreadPool::globalInstance()->start(task);
}

ProcessOperation::~ProcessOperation() {
    qDebug()<<"distructor process operation called";
}

void ProcessOperation::onLoginError(QWebSocket* socket, QString errorMessage)
{
    qDebug()<<"Funzione onLoginError viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

     QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,
                BuilderMessage::MessageLoginError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallenge(QWebSocket* socket, QString salt, QString challenge)
{
    qDebug()<<"Funzione onMessageChallange viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                  data,BuilderMessage::MessageChallege(salt, challenge));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallengePassed(QWebSocket * socket, QString d)
{
    qDebug()<<"Funzione onMessageChallangePassed viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageChallegePassed(d));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountCreationError(QWebSocket * socket, QString errorMessage){

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountConfirmed(QWebSocket* socket, QString message)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed(message));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onOpenDirOfClient(QWebSocket *socket, QJsonArray files)
{
    qDebug()<<"Funzione onOpenDirClient viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageOpenDirOfClient(files));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountUpdateSuccess(QWebSocket *socket, QString message)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateSuccess(message));

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onPersonalDataOfClient(QWebSocket* socket, QString username, QString nickname, QImage image)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountInfo(username, nickname,image));

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onFileCreationError(QWebSocket *socket, QString errorMessage)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageFileCreationError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onFileDeletionError(QWebSocket *socket, QString errorMessage)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageFileDeletionError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onOpenFile(QWebSocket *socket, QString fileName, QByteArray contentFile)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageHeaderFile(fileName));

    if(contentFile != nullptr)
        BuilderMessage::MessageSendToClient(data,contentFile);

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onSocketAbort(QWebSocket *clientSocket)
{
    qDebug()<<clientSocket<<" invocazione metodo socket abort   "<<QThread::currentThread();

    QSharedPointer<Client> client = clients[clientSocket];
    QString ip;

    if(clientSocket->isValid()){
        ip=clientSocket->peerAddress().toString();
        qDebug()<<"socket abort thread "<<clientSocket->thread()->currentThread();
        clientSocket->close(
                    QWebSocketProtocol::CloseCodeBadOperation);
        clientSocket->deleteLater();
    }
    clients.remove(clientSocket);
    if (client->isLogged())
    {
        client->logout();
        emit printUiServer("Eject "+client->getUsername()+" ip: "+ip);
    }
    else
        emit printUiServer("Shutdown connection client: "+ip);

}
