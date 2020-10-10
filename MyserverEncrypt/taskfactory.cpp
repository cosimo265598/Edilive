#include "taskfactory.h"

TaskFactory::TaskFactory(QObject *parent) : QObject(parent)
{

}

TaskFactory *TaskFactory::createTask(TypeOperation type){
    switch (type)
        {
            //case TypeOperation::LoginRequest:			return "LoginRequest";
            //case TypeOperation::LoginChallenge:         return "LoginChallenge";
            //case TypeOperation::LoginUnlock:			return "LoginUnlock";
            //case TypeOperation::LoginGranted:			return "LoginGranted";
            //case TypeOperation::LoginError:             return "LoginError";
            case TypeOperation::AccountCreate:          return new class AccountCreate();
            case TypeOperation::AccountUpdate:          return new class AccountUpdate();
            //case TypeOperation::AccountConfirmed:		return "AccountConfirmed";
            //case TypeOperation::AccountError:			return "AccountError";
            //case TypeOperation::Simplemessage:          return "Simplemessage";
            //case TypeOperation::OpenDirectory:          return "OpenDirecory";
            //case TypeOperation::OpenFile:               return "OpenFile";
            //case TypeOperation::CreateFile:             return "CreateFile";
            //case TypeOperation::ErrorFile:              return "ErrorFile";
            //case TypeOperation::Logout:                 return "Logout";
            //case TypeOperation::Failure:                return "Failure";
            //case TypeOperation::ProfileData:            return "ProfileData";
            //case TypeOperation::DeleteFile:             return "DeleteFile";
            // add other case below
            default:                                    return nullptr;
        }
}
