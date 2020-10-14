#include "tasks.h"

Tasks::Tasks(QObject *parent, QWebSocket *clientSocket,
             QJsonObject request, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation type) :
    QObject(parent),
    clients(clients),
    users(users),
    clientSocket(clientSocket),
    type(type),
    request(request),
    threadId(QString::number((quintptr)QThread::currentThreadId()))
{

    qDebug() << "creato";
    clientSocket->moveToThread(QThread::currentThread());
    db.open(defaultnamedb,threadId,nullptr);
}

void Tasks::serverLoginRequest(QWebSocket* clientSocket, QJsonObject request){

    QString username = request["username"].toString();

    QSharedPointer<Client> client = clients[clientSocket];
    QByteArray data;

    //qDebug() << &database;
    //qDebug() << database.getMaxUserID();

    qDebug() << QThread::currentThread();
    this->thread()->currentThread()->msleep(15000);

    if(users[username].isEmpty()){            // utente non registrato, non presente nel db.
        //ui->commet->appendPlainText("Client not Registered "+username);
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Client not registered." + username));

        clientSocket->sendBinaryMessage(data);
}
    /*
    try {
        qDebug() << &database;
        UserData user(database.readUser(username));

        if(user.isEmpty()){            // utente non registrato, non presente nel db.
            //ui->commet->appendPlainText("Client not Registered "+username);
            BuilderMessage::MessageSendToClient(
                        data,
                        BuilderMessage::MessageLoginError("Client not registered."));

            clientSocket->sendBinaryMessage(data);
            //socketAbort(clientSocket);



        }else{
            for (QString docUri : database.readUserDocuments(user.getUsername()))
                user.addDocument(docUri);
            users.insert(user.getUsername(), user);
        }
    }  catch (DatabaseReadException& re) {
        users.remove(username);
        //ui->commet->appendPlainText("Databaseread problem during the query excution");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Error database sever"));
        clientSocket->sendBinaryMessage(data);
        //socketAbort(clientSocket);
        return;
    }


    if (users.contains(username))
    {
        if (client->isLogged()){
            //ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
            BuilderMessage::MessageSendToClient(
                        data,
                        BuilderMessage::MessageLoginError("Client: "+client->getUsername()+" is already logged in."));
            clientSocket->sendBinaryMessage(data);
            return;
        }
        // client not logged
        BuilderMessage::MessageSendToClient(
                      data,BuilderMessage::MessageChallege(
                      QString(users[username].getSalt()) ,
                      QString(client->challenge(&users[username]))));

        clientSocket->sendBinaryMessage(data);

    }

    else {
        // send message utente non registrato
        ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Client not registered."));

        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
    }

    */
}


void Tasks::test(QJsonObject m){
    qDebug() << m["username"].toString() << QThread::currentThreadId();
}

void Tasks::run(){
    switch (type) {
        //case AccountCreate: this->serverAccountCreate(clientSocket,request);break;
        case LoginRequest: this->serverLoginRequest(clientSocket,request);break;
        default: qDebug() << "No Task";

    }
}

Tasks::~Tasks()
{
    this->db.removeDatabase(threadId);
}
