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
    this->database.open(defaultnamedb,threadId,nullptr);
}

void Tasks::serverLoginRequest(){

    QString username = request["username"].toString();

    QSharedPointer<Client> client = clients[this->clientSocket];

    QByteArray data;

    qDebug() << QThread::currentThread();
    //this->thread()->currentThread()->msleep(15000);

    if(users[username].isEmpty()){            // utente non registrato, non presente nel db.
        //ui->commet->appendPlainText("Client not Registered "+username);
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Client not registered." + username));

        clientSocket->sendBinaryMessage(data);
    }

    try {
        qDebug() << "Try";
        UserData user(database.readUser(username));
        qDebug() << "Empty?";

        if(user.isEmpty()){            // utente non registrato, non presente nel db.
            //ui->commet->appendPlainText("Client not Registered "+username);
            BuilderMessage::MessageSendToClient(
                        data,
                        BuilderMessage::MessageLoginError("Client not registered."));

            clientSocket->sendBinaryMessage(data);
            //socketAbort(clientSocket);


        }else{
            qDebug() << "leggo file";

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
        clientSocket->moveToThread(QCoreApplication::instance()->thread());
        return;
    }

    qDebug() << "Ok go on";

    qDebug() << users.size();

    if (users.contains(username))
    {
        if (client.get()->isLogged()){
            //ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
            BuilderMessage::MessageSendToClient(
                        data,
                        BuilderMessage::MessageLoginError("Client: "+client->getUsername()+" is already logged in."));
            clientSocket->sendBinaryMessage(data);
            clientSocket->moveToThread(QCoreApplication::instance()->thread());
            return;
        }
        // client not logged
        BuilderMessage::MessageSendToClient(
                      data,BuilderMessage::MessageChallege(
                      QString(users[username].getSalt()) ,
                      QString(client->challenge(&users[username]))));
        qDebug() << "prima di send";
        clientSocket->sendBinaryMessage(data);
        qDebug() << "dopo send";
        clientSocket->moveToThread(QCoreApplication::instance()->thread());
    }

    else {
        // send message utente non registrato
        //ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Client not registered."));

        clientSocket->sendBinaryMessage(data);
        //socketAbort(clientSocket);
        clientSocket->moveToThread(QCoreApplication::instance()->thread());
    }


}


void Tasks::test(QJsonObject m){
    qDebug() << m["username"].toString() << QThread::currentThreadId();
}

void Tasks::run(){
    switch (type) {
        //case AccountCreate: this->serverAccountCreate(clientSocket,request);break;
        case LoginRequest: this->serverLoginRequest();break;
        default: qDebug() << "No Task";

    }
}

Tasks::~Tasks()
{
    this->database.removeDatabase(threadId);
}
