#ifndef TASKS_H
#define TASKS_H

#include <QObject>
#include <QRunnable>
#include <QWebSocket>
#include <QThreadPool>
#include <QMap>

#include "client.h"
#include "serverdatabase.h"
#include "buildermessage.h"
#include "serverexception.h"

class Tasks : public QObject, public QRunnable
{
    Q_OBJECT
public:
    Tasks(QObject *parente);
    static void serverAccountCreate(QWebSocket *clientSocket, QString username, QString password, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, ServerDatabase& database);
private:

signals:

};

#endif // TASKS_H
