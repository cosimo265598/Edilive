#include "workspace.h"

QList<QSharedPointer<Client>> Workspace::getClients() const
{
    return clients.values();
}

Workspace::Workspace(QObject *parent, QString path) : QObject(parent), path(path), sf(nullptr)
{

    QFileInfo info(path);
     qDebug() << "Created new workspace with filename="<<path;
     sf = new SharedFile(info.fileName(),info.owner());
     sf -> readExistingFile(path);
}

void Workspace::addClient(QWebSocket* clientSocket, QSharedPointer<Client> client)
{
    qDebug()<<"Sono nella add client";
    this->clients.insert(clientSocket, client);
    qDebug() << "numero clients collegati: " << clients.size() << "      clients: " << clients;
}

void Workspace::removeClient(QWebSocket* socket)
{
    this->clients.remove(socket);
    qDebug() << "Nel workspace ci sono ancora: " << clients.size();
}

SharedFile *Workspace::getSharedFile()
{
    return sf;
}
