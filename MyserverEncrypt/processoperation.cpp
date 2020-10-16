#include "mainwindow.h"
#include "processoperation.h"
#include <iostream>
#include <QObject>
#include <memory>

ProcessOperation* ProcessOperation::instance;
std::once_flag    ProcessOperation::inited;

ProcessOperation::ProcessOperation(QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users
):QObject(parent),
    clients(clients),
    users(users)
{
}

Tasks *ProcessOperation::createTask(QObject *parent,
                                    QJsonObject request, QWebSocket* socket, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation typeOp)
{
    Tasks *task = new Tasks(nullptr, request, socket, clients, users, typeOp);

    connect(task,&Tasks::loginError, this, &ProcessOperation::onLoginError);
    connect(task,&Tasks::messageChallenge, this, &ProcessOperation::onMessageChallenge);
    connect(task,&Tasks::messageChallegePassed, this, &ProcessOperation::onMessageChallengePassed);
    connect(task,&Tasks::accountCreationError, this, &ProcessOperation::onAccountCreationError);
    connect(task,&Tasks::accountConfirmed, this, &ProcessOperation::onAccountConfirmed);
    connect(task,&Tasks::openDirOfClient, this, &ProcessOperation::onOpenDirOfClient);
    connect(task,&Tasks::personalDataOfClient, this, &ProcessOperation::onPersonalDataOfClient);
    connect(task,&Tasks::accountUpdateSuccess, this, &ProcessOperation::onAccountUpdateSuccess);
    connect(task,&Tasks::fileCreationError, this, &ProcessOperation::onFileCreationError);
    connect(task,&Tasks::fileDeletionError, this, &ProcessOperation::onFileDeletionError);
    connect(task,&Tasks::openFile, this, &ProcessOperation::onOpenFile);

    return task;
}


void ProcessOperation::process(QWebSocket* socket, QJsonObject request)
{
    TypeOperation typeOp = (TypeOperation)(request["type"].toInt());

    Tasks *task = this->createTask(nullptr, request, socket, clients, users, typeOp);

    QThreadPool::globalInstance()->start(task);
}

ProcessOperation::~ProcessOperation() {
    qDebug()<<"distructor process operation called";
}

void ProcessOperation::onLoginError(QWebSocket* socket, QString errorMessage)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,
                BuilderMessage::MessageLoginError(errorMessage));

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallenge(QWebSocket* socket, QString salt, QString challenge)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                  data,BuilderMessage::MessageChallege(salt, challenge));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallengePassed(QWebSocket * socket, QString d)
{
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

/*
QString ProcessOperation::checkTypeOperationGranted(TypeOperation type){
    switch (type)
        {
            case TypeOperation::LoginRequest:			return "LoginRequest";
            case TypeOperation::LoginChallenge:         return "LoginChallenge";
            case TypeOperation::LoginUnlock:			return "LoginUnlock";
            case TypeOperation::LoginGranted:			return "LoginGranted";
            case TypeOperation::LoginError:             return "LoginError";
            case TypeOperation::AccountCreate:          return "AccountCreate";
            case TypeOperation::AccountUpdate:          return "AccountUpdate";
            case TypeOperation::AccountConfirmed:		return "AccountConfirmed";
            case TypeOperation::AccountError:			return "AccountError";
            case TypeOperation::Simplemessage:          return "Simplemessage";
            case TypeOperation::OpenDirectory:          return "OpenDirecory";
            case TypeOperation::OpenFile:               return "OpenFile";
            case TypeOperation::CreateFile:             return "CreateFile";
            case TypeOperation::ErrorFile:              return "ErrorFile";
            case TypeOperation::Logout:                 return "Logout";
            case TypeOperation::Failure:                return "Failure";
            case TypeOperation::ProfileData:            return "ProfileData";
            case TypeOperation::DeleteFile:             return "DeleteFile";
            // add other case below
            default:                                    return QString();
        }
}

*/


