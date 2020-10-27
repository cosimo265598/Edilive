#include "mainwindow.h"
#include "processoperation.h"
#include <iostream>
#include <QObject>
#include <memory>

ProcessOperation* ProcessOperation::instance;
std::once_flag    ProcessOperation::inited;

ProcessOperation::ProcessOperation(
        QObject *parent, QMap<QWebSocket*, QSharedPointer<Client>>& clients,
        QMap<QString, UserData>& users,
        QMap<QString, QSharedPointer<Workspace>>& workspaces):
    QObject(parent),
    clients(clients),
    users(users),
    workspaces(workspaces)
{
}

Tasks *ProcessOperation::createTask(
        QObject *parent,
        QJsonObject request,
        QWebSocket* socket,
        QMap<QWebSocket*, QSharedPointer<Client>>& clients,
        QMap<QString, UserData>& users,
        QMap<QString, QSharedPointer<Workspace>>& workspaces,
        TypeOperation typeOp,
        Ui::MainWindow* ui)
{
    Tasks *task = new Tasks(nullptr, request, socket, clients, users, workspaces, typeOp,ui);

    connect(task,&Tasks::loginError, this, &ProcessOperation::onLoginError,Qt::DirectConnection);
    connect(task,&Tasks::messageChallenge, this, &ProcessOperation::onMessageChallenge,Qt::DirectConnection);
    connect(task,&Tasks::messageChallegePassed, this, &ProcessOperation::onMessageChallengePassed,Qt::DirectConnection);
    connect(task,&Tasks::accountCreationError, this, &ProcessOperation::onAccountCreationError,Qt::DirectConnection);
    connect(task,&Tasks::accountConfirmed, this, &ProcessOperation::onAccountConfirmed,Qt::DirectConnection);
    connect(task,&Tasks::openDirOfClient, this, &ProcessOperation::onOpenDirOfClient,Qt::DirectConnection);
    connect(task,&Tasks::personalDataOfClient, this, &ProcessOperation::onPersonalDataOfClient,Qt::DirectConnection);
    connect(task,&Tasks::accountUpdateSuccess, this, &ProcessOperation::onAccountUpdateSuccess,Qt::DirectConnection);
    connect(task,&Tasks::fileCreationError, this, &ProcessOperation::onFileCreationError,Qt::DirectConnection);
    connect(task,&Tasks::fileDeletionError, this, &ProcessOperation::onFileDeletionError,Qt::DirectConnection);
    connect(task,&Tasks::openFile, this, &ProcessOperation::onOpenFile,Qt::DirectConnection);
    connect(task,&Tasks::insertionChar, this, &ProcessOperation::onInsertionChar, Qt::DirectConnection);
    connect(task,&Tasks::deletionChar, this, &ProcessOperation::onDeletionChar, Qt::DirectConnection);
    connect(task,&Tasks::removeClientFromWorkspace, this, &ProcessOperation::onRemoveClientFromWorkspace, Qt::DirectConnection);

    connect(task,&Tasks::socketAbort, this, &ProcessOperation::onSocketAbort,Qt::QueuedConnection);
    connect(task,&Tasks::printUiServer,dynamic_cast<MainWindow*>(this->parent()),&MainWindow::printUiServer,Qt::QueuedConnection);
    connect(this,&ProcessOperation::printUiServer,dynamic_cast<MainWindow*>(this->parent()),&MainWindow::printUiServer,Qt::QueuedConnection);


    return task;
}


void ProcessOperation::process(QWebSocket* socket, QJsonObject request, Ui::MainWindow* ui)
{
    TypeOperation typeOp = (TypeOperation)(request["type"].toInt());

    Tasks *task = this->createTask(nullptr, request, socket, clients, users, workspaces, typeOp,ui);

    QThreadPool::globalInstance()->start(task);
}

ProcessOperation::~ProcessOperation() {
    qDebug()<<"distructor process operation called";
}

void ProcessOperation::onLoginError(QWebSocket* socket, QString errorMessage)
{
    qDebug()<<"Funzione onLoginError viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

     QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,
                BuilderMessage::MessageLoginError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallenge(QWebSocket* socket, QString salt, QString challenge)
{
    qDebug()<<"Funzione onMessageChallange viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                  data,BuilderMessage::MessageChallege(salt, challenge));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onMessageChallengePassed(QWebSocket * socket, QString d)
{
    qDebug()<<"Funzione onMessageChallangePassed viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageChallegePassed(d));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountCreationError(QWebSocket * socket, QString errorMessage){

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountCreationError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountConfirmed(QWebSocket* socket, QString message)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed(message));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onOpenDirOfClient(QWebSocket *socket, QJsonArray files)
{
    qDebug()<<"Funzione onOpenDirClient viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageOpenDirOfClient(files));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onAccountUpdateSuccess(QWebSocket *socket, QString message)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateSuccess(message));

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onPersonalDataOfClient(QWebSocket* socket, QString username, QString nickname, QImage image)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountInfo(username, nickname,image));

    socket->sendBinaryMessage(data);
}

void ProcessOperation::onFileCreationError(QWebSocket *socket, QString errorMessage)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageFileCreationError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onFileDeletionError(QWebSocket *socket, QString errorMessage)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageFileDeletionError(errorMessage));
    socket->sendBinaryMessage(data);
}

void ProcessOperation::onOpenFile(QWebSocket* socket, QString fileName)
{

    Client *client = clients[socket].get();
    QByteArray data;
    BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageHeaderFile(fileName, client->getUsername(),  workspaces[fileName].get()->getClients())); //DA SISTEMARE CON VERO CREATORE
    BuilderMessage::MessageSendToClient(data, workspaces[fileName].get()->getSharedFile());
    socket->sendBinaryMessage(data);

    qDebug()<< "<MAIN WINDOWS> voglio updatare tutti ";
    for(QSharedPointer<Client> cl : workspaces[fileName]->getClients()){
        if(cl->getUsername().compare(client->getUsername())!=0){
            QByteArray data;
            BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageInsertClientWorkspace(client->getUser()->getUsername(),client->getUser()->getNickname(),client->getUser()->getIcon())); //DA SISTEMARE CON VERO CREATORE
            cl->getSocket()->sendBinaryMessage(data);
        }
        qDebug()<<data;
    }
}

void ProcessOperation::onInsertionChar(QWebSocket *clientSocket, Symbol s){

    QString caller =clients[clientSocket]->getUsername();
    /*
    workspaces[QString::fromStdString(s.getSite())].get()->insertChar(s, "server", clientSocket, caller);
    */
    QSharedPointer<Workspace> w =  workspaces[QString::fromStdString(s.getSite())];

    SharedFile *sf = w->getSharedFile();

    //AREA DEBUG
    qDebug() << "INSERIMENTO DA CLIENT";
    qDebug() << "File pre inserimento: " << QString::fromStdString(sf->to_string());
    for(Symbol s : sf->getSymbols()){
        std::string posfstringa="";
        for(int i=0; i<s.getPosFraz().size(); i++){
            std::stringstream ss;
            ss << s.getPosFraz()[i];
            std::string str = ss.str();
            if(i==s.getPosFraz().size()-1)
                posfstringa.append(str);
            else
                posfstringa.append(str).append("-");
        }

        qDebug() << s.getCar() << " con posfraz=" << QString::fromStdString(posfstringa) ;
    }
    //fine AREA DEBUG

    int esito = sf->localInsert(s, "server");
    qDebug()<< "Conflitto: "<<esito;
    if(esito==1){//conflitto
        qDebug()<<"Sono nella zona di conflitto perchè esito="<<esito;
        std::vector<Symbol> simboli = sf->getSymbols();
        std::vector<int> nuovapos;
        for(int i=0; i<simboli.size(); i++)
            if(simboli[i].getFloatPosFraz()==s.getFloatPosFraz()){
                nuovapos=simboli[i+1].getPosFraz();
                break;
            }
        Symbol news(s.getCar(),s.getSite(), nuovapos, s.getId());
        QByteArray data1;
        BuilderMessage::MessageSendToClient(
                    data1,BuilderMessage::MessageDelete(s.getCar(), s.getPosFraz(), QString::fromStdString(s.getId()), QString::fromStdString(s.getSite())));

        clientSocket->sendBinaryMessage(data1);
        for(QSharedPointer<Client> cl : w->getClients()){
                QByteArray data;
                BuilderMessage::MessageSendToClient(
                            data,BuilderMessage::MessageInsert(s.getCar(), news.getPosFraz(), QString::fromStdString(s.getId()), QString::fromStdString(s.getSite())/*,"no"*/));

                cl->getSocket()->sendBinaryMessage(data);
        }
    }
    else{
        for(QSharedPointer<Client> cl : w->getClients()){
            if(cl->getUsername()!=caller){
                QByteArray data;
                BuilderMessage::MessageSendToClient(
                            data,BuilderMessage::MessageInsert(s.getCar(), s.getPosFraz(), QString::fromStdString(s.getId()), QString::fromStdString(s.getSite())/*,"no"*/));

                cl->getSocket()->sendBinaryMessage(data);
            }
        }
    }
    //AREA DEBUG
    qDebug() << "File post inserimento: " << QString::fromStdString(sf->to_string());
    for(Symbol s : sf->getSymbols()){
        std::string posfstringa="";
        for(int i=0; i<s.getPosFraz().size(); i++){
            std::stringstream ss;
            ss << s.getPosFraz()[i];
            std::string str = ss.str();
            if(i==s.getPosFraz().size()-1)
                posfstringa.append(str);
            else
                posfstringa.append(str).append("-");
        }

        qDebug() << s.getCar() << " con posfraz=" << QString::fromStdString(posfstringa) ;
    }
    //fine AREA DEBUG
}

void ProcessOperation::onDeletionChar(QWebSocket *clientSocket, Symbol s){
    QString caller =clients[clientSocket]->getUsername();
    //workspaces[QString::fromStdString(s.getSite())].get()->localErase(s, "server", caller);

    QSharedPointer<Workspace> w =  workspaces[QString::fromStdString(s.getSite())];
    SharedFile *sf = w->getSharedFile();


    sf->localErase(s, "server");
    //INFORMA TUTTI I CLIENT TRANNE IL CHIAMANTE DELLA CANCELLAZIONE
    for(QSharedPointer<Client> cl : w->getClients()){
        if(cl->getUsername()!=caller){
            QByteArray data;
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageDelete(s.getCar(), s.getPosFraz(), QString::fromStdString(s.getId()), QString::fromStdString(s.getSite())));

            cl->getSocket()->sendBinaryMessage(data);
        }
    }
}

void ProcessOperation::onRemoveClientFromWorkspace(QWebSocket *clientSocket, QString fileName)
{
    workspaces[fileName]->removeClient(clientSocket);
    QString caller =clients[clientSocket]->getUsername();
    for(auto cl : workspaces[fileName]->getClients()){
        qDebug() << "Mi devo rimuovere dagli altri";
        if(cl->getUsername()!=caller){
            QByteArray data;
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageRemoveClientFromWorkspace(caller));
            cl->getSocket()->sendBinaryMessage(data);
        }
    }
}

void ProcessOperation::onAccountUpdateError(QWebSocket *clientSocket, QString error)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateError(error));
    clientSocket->sendBinaryMessage(data);
}

void ProcessOperation::onSocketAbort(QWebSocket *clientSocket)
{
    qDebug()<<clientSocket<<" invocazione metodo socket abort   "<<QThread::currentThread();

    QSharedPointer<Client> client = clients[clientSocket];
    QString ip;

    if(clientSocket->isValid()){
        ip=clientSocket->peerAddress().toString();
        qDebug()<<"socket abort thread "<<clientSocket->thread()->currentThread();
        clientSocket->close(
                    QWebSocketProtocol::CloseCodeBadOperation);
        clientSocket->deleteLater();
    }
    clients.remove(clientSocket);
    if (client->isLogged())
    {
        client->logout();
        emit printUiServer("Eject "+client->getUsername()+" ip: "+ip);
    }
    else
        emit printUiServer("Shutdown connection client: "+ip);

}
