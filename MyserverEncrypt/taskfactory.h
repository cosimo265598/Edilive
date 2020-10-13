#ifndef TASKFACTORY_H
#define TASKFACTORY_H

#include <QObject>
#include <QRunnable>
#include <QDebug>

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
