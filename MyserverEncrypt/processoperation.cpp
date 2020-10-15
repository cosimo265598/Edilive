#include "mainwindow.h"
#include "processoperation.h"
#include <iostream>
#include <QObject>
#include <memory>

ProcessOperation* ProcessOperation::instance;
std::once_flag    ProcessOperation::inited;

ProcessOperation::ProcessOperation(QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users
):QObject(parent), clients(clients), users(users)
{
    // signal - slot for login method- registration and first phase untill the text edit

    connect(this,&ProcessOperation::loginRequest,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::serverLoginRequest,Qt::DirectConnection);
    connect(this,&ProcessOperation::loginUnlock,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::serverLoginUnlock,Qt::DirectConnection);
    /*
     *  Account method */

    connect(this,&ProcessOperation::accountCreate,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::serverAccountCreate,Qt::DirectConnection);

    /*
     *  test message method */

    connect(this,&ProcessOperation::SimpleMessage,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::SimpleTextMessageTest,Qt::DirectConnection);
    /*
     * client file
     */
    connect(this,&ProcessOperation::OpenDirOfClient,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::OpenDirOfClient,Qt::DirectConnection);
    connect(this,&ProcessOperation::CreateFileForClient,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::CreateFileForClient,Qt::DirectConnection);
    connect(this,&ProcessOperation::OpenFileForClient,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::OpenFileForClient,Qt::DirectConnection);
    connect(this,&ProcessOperation::DeleteFileForClient,
                dynamic_cast<MainWindow*>(this->parent()),
                &MainWindow::DeleteFileForClient,Qt::DirectConnection);

    /*
     * personal data
     */
    connect(this,&ProcessOperation::PersonalDataOfClient,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::PersonalDataOfClient,Qt::DirectConnection);

    connect(this, &ProcessOperation::accountUpdate,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::updateProfileClient,Qt::DirectConnection);

}


void ProcessOperation::process(QWebSocket* socket, QJsonObject data)
{
    TypeOperation typeOp = (TypeOperation)(data["type"].toInt());

    switch (typeOp)
    {
        case AccountCreate:{
            QString user    =data.value("username").toString();
            QString password=data.value("password").toString();

            qDebug() << "disassocio thread";
            qDebug() << "chiamo runnable";

            //return new Tasks(nullptr, socket, data,  database,clients, users, AccountCreate);

            emit accountCreate(socket,user,password);
            break;
        }

        case Simplemessage:{
            emit SimpleMessage(socket,QString("hello message for testing sended\n"));
            break;
        }
        case OpenDirectory:{
            emit OpenDirOfClient(socket);
            break;
        }
        case CreateFile:{
            QString file    =data.value("nomefile").toString();
            emit CreateFileForClient(socket,file);
            break;
        }
        case OpenFile:{
            QString file    =data.value("nomefile").toString();
            emit OpenFileForClient(socket,file);
            break;
        }
        case ProfileData:{
            emit PersonalDataOfClient(socket);
            break;
        }

        case DeleteFile:{
            QString file    =data.value("nomefile").toString();
            emit DeleteFileForClient(socket,file);
            break;
        }

        case AccountUpdate:{
            QString user    =data.value("nickname").toString();
            QString password=data.value("password").toString();
            QString serializedImage=data.value("icon").toString();

            emit accountUpdate(socket,user,password, serializedImage);
            break;
        }

        default:		// Wrong message type already addressed in readMessage,
        break;
            //return;		// no need to handle error again
    }

    qDebug() << "Main thread  " << QThread::currentThread()->currentThreadId();

    Tasks *task = new Tasks(nullptr, data, socket, clients, users, typeOp);

    connect(task,&Tasks::errorMessage, this, &ProcessOperation::onMessageError);
    connect(task,&Tasks::messageChallenge, this, &ProcessOperation::onMessageChallenge);
    connect(task,&Tasks::messageChallegePassed, this, &ProcessOperation::onMessageChallengePassed);

    QThreadPool::globalInstance()->start(task);


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
ProcessOperation::~ProcessOperation() {
    qDebug()<<"distructor process operation called";
}

void ProcessOperation::onMessageError(QWebSocket* socket, QString errorMessage)
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


