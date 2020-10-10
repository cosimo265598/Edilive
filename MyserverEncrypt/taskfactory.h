#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <QObject>
#include <QRunnable>
#include <QDebug>

enum TypeOperation : quint16
{
    // Tests
    Simplemessage,
    // Login messages
    LoginRequest,
    LoginChallenge,
    LoginUnlock,
    LoginGranted,
    LoginError,

    // Account messages
    AccountCreate,
    AccountUpdate,
    AccountConfirmed,
    AccountError,

    // File Management
    OpenDirectory,
    OpenFile,
    CreateFile,
    ErrorFile,
    // Logout message
    Logout,
    // Others
    Failure,

    // personal image
    ProfileData,
    DeleteFile
    // add other enum below, and do not change the order of list

};

class TaskFactory : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskFactory(QObject *parent = nullptr);
    static TaskFactory *createTask(TypeOperation op);

signals:

};

class AccountCreate : public TaskFactory{
public:
    virtual void run() override{
        qDebug() << "AccountCreate";
    }
};
class AccountUpdate : public TaskFactory{
public:
    virtual void run() override{
        qDebug() << "AccountUpdate";
    }
};

#endif // TASKFACTORY_H
