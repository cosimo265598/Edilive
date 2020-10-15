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


QRunnable *ProcessOperation::process(TypeOperation message, QWebSocket* socket, QJsonObject data)
{

    //socket->moveToThread(nullptr);
    switch (message)
    {

         /* LOGIN MESSAGES */

        case LoginRequest:{

            //QString username    =data.value("username").toString();

            //users.insert("ciao", UserData("ciao",2,"ciao","ciao",QImage()));

            return new Tasks(nullptr, socket, data,clients, users, LoginRequest);
            //emit loginRequest(socket,username);
            break;
        }
        case LoginUnlock:{
            QString token    =data.value("challange").toString();
            emit loginUnlock(socket,token);
            break;
        }
        case AccountCreate:{
            QString user    =data.value("username").toString();
            QString password=data.value("password").toString();

            qDebug() << "disassocio thread";
            socket->moveToThread(nullptr);
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
            return nullptr;		// no need to handle error again
    }
}


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
ProcessOperation::~ProcessOperation() {
    qDebug()<<"distructor process operation called";
}
