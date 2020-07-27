#include "mainwindow.h"
#include "processoperation.h"
#include <iostream>
#include <QObject>
#include <memory>


ProcessOperation::ProcessOperation(QObject *parent):QObject(parent)
{
    //this->wind =  QSharedPointer<MainWindow>( qobject_cast<MainWindow*>(this->parent()) ) ;
    /*
    //aggancio il segnale login request della classe process operation a quella presente nel server
    connect(this, &ProcessOperation::loginRequest, s, &TcpServer::serveLoginRequest, Qt::DirectConnection);
    connect(this, &ProcessOperation::loginUnlock, s, &TcpServer::authenticateUser, Qt::DirectConnection);

    connect(this, &ProcessOperation::accountCreate, s, &TcpServer::createAccount, Qt::DirectConnection);
    connect(this, &ProcessOperation::accountUpdate, s, &TcpServer::updateAccount, Qt::DirectConnection);
    connect(this, &ProcessOperation::userLogout, s, &TcpServer::logoutClient, Qt::DirectConnection);
    */

    connect(
            this,
            &ProcessOperation::SimpleMessage,
            dynamic_cast<MainWindow*>(this->parent()),
            &MainWindow::SimpleTextMessageTest,
            Qt::DirectConnection);
}

void ProcessOperation::process(TypeOperation message, QWebSocket* socket)
{

    switch (message)
    {

         /* LOGIN MESSAGES */

        case LoginRequest:{
            // do qualcosa
            // attraverso segnali , e metodi implemntati nella classe server
            // esempio emit login request ->serverLogin
            break;
        }
        case Simplemessage:{
             emit SimpleMessage(socket,QString("hello message test sended\n"));
            break;
        }

        default:		// Wrong message type already addressed in readMessage,
            return;		// no need to handle error again
    }
}


QString ProcessOperation::typeOp(TypeOperation type){
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
        case TypeOperation::Logout:                 return "Logout";
        case TypeOperation::Simplemessage:          return "Simplemessage";

        default:		return "UnknownType ";
    }
}
ProcessOperation::~ProcessOperation() { }
