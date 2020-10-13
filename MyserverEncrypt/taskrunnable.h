#ifndef TASKRUNNABLE_H
#define TASKRUNNABLE_H

#include <QObject>
#include <QRunnable>

class TaskRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskRunnable(QObject *parent = nullptr);

signals:

};

#endif // TASKRUNNABLE_H
