#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QObject>
#include <QWebSocket>
#include <QFile>
#include <QFileInfo>

#include "SharedFile.h"
#include "Symbol.h"
#include "client.h"

class Client;

class Workspace : public QObject
{
    Q_OBJECT
private:
    QMap<QWebSocket*, QSharedPointer<Client>> clients;
    QString path;
    SharedFile *sf;

public:
    Workspace(QObject *parent = nullptr, QString path = "");
    void addClient (QWebSocket* , QSharedPointer<Client>);
    void removeClient(QWebSocket* );

    SharedFile *getSharedFile();

    //void insertChar(Symbol s, QString side, QWebSocket *clientSocket, QString caller);
    //void localErase(Symbol s, QString side, QString caller);

    QList<QSharedPointer<Client>> getClients() const;

signals:

};

#endif // WORKSPACE_H
