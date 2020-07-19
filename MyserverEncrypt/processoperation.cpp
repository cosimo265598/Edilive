#include "processoperation.h"

ProcessOperation::ProcessOperation(QWebSocketServer *s)
{
    /*
    //aggancio il segnale login request della classe process operation a quella presente nel server
    connect(this, &ProcessOperation::loginRequest, s, &TcpServer::serveLoginRequest, Qt::DirectConnection);
    connect(this, &ProcessOperation::loginUnlock, s, &TcpServer::authenticateUser, Qt::DirectConnection);

    connect(this, &ProcessOperation::accountCreate, s, &TcpServer::createAccount, Qt::DirectConnection);
    connect(this, &ProcessOperation::accountUpdate, s, &TcpServer::updateAccount, Qt::DirectConnection);


    connect(this, &ProcessOperation::userLogout, s, &TcpServer::logoutClient, Qt::DirectConnection);
    */
}

void ProcessOperation::process(TypeOperation message, QSslSocket* socket)
{

    switch (message)
    {

        /* LOGIN MESSAGES */

    case LoginRequest:
    {
        // do qualcosa
        // attraverso segnali , e meotodi implemntati nella classe server
        // esempio emit login request ->serverLogin

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
        case TypeOperation::LoginChallenge:		return "LoginChallenge";
        case TypeOperation::LoginUnlock:			return "LoginUnlock";
        case TypeOperation::LoginGranted:			return "LoginGranted";
        case TypeOperation::LoginError:			return "LoginError";
        case TypeOperation::AccountCreate:		return "AccountCreate";
        case TypeOperation::AccountUpdate:		return "AccountUpdate";
        case TypeOperation::AccountConfirmed:		return "AccountConfirmed";
        case TypeOperation::AccountError:			return "AccountError";
        case TypeOperation::Logout:				return "Logout";

        default:		return "UnknownType ";
    }
}
ProcessOperation::~ProcessOperation() { }
