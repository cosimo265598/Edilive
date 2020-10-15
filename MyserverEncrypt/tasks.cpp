#include "tasks.h"

Tasks::Tasks(QObject *parent,
             QJsonObject request, QWebSocket* socket, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, TypeOperation typeOp) :
    QObject(parent),
    clients(clients),
    users(users),
    typeOp(typeOp),
    socket(socket),
    request(request),
    threadId(QString::number((quintptr)QThread::currentThreadId()))
{

    qDebug() << "creato";
    qDebug() << "ThreadPool  " << QThread::currentThread()->currentThreadId();
    //this->database.open(defaultnamedb,threadId,nullptr);
}

void Tasks::serverLoginRequest(){

    ServerDatabase database;
    database.open(defaultnamedb,threadId,nullptr);

    qDebug() << users.size();
    QString username = request["username"].toString();

    QSharedPointer<Client> client = clients[socket];

    qDebug() << QThread::currentThread();
    //this->thread()->currentThread()->msleep(15000);

    if(users[username].isEmpty()){            // utente non registrato, non presente nel db.
        //ui->commet->appendPlainText("Client not Registered "+username);
        emit errorMessage(socket, "Client not registered." + username);
    }

    try {
        qDebug() << "Try";
        UserData user(database.readUser(username));

        qDebug() << user.getUserId();

        if(user.isEmpty()){            // utente non registrato, non presente nel db.
            //ui->commet->appendPlainText("Client not Registered "+username);
            emit errorMessage(socket, "Client not registered.");
        }else{
            qDebug() << "leggo file";

            for (QString docUri : database.readUserDocuments(user.getUsername()))
                user.addDocument(docUri);
            users.insert(user.getUsername(), user);
        }

    }  catch (DatabaseReadException& re) {
        users.remove(username);
        //ui->commet->appendPlainText("Databaseread problem during the query excution");
        emit errorMessage(socket, "Error database sever");
        return;
    }

    qDebug() << "Ok go on";
    qDebug() << users.size();

    if (users.contains(username))
    {
        if (client.get()->isLogged()){
            //ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
            emit errorMessage(socket, "Client: "+client->getUsername()+" is already logged in.");
            return;
        }
        // client not logged
        emit messageChallenge(socket, QString(users[username].getSalt()), QString(client->challenge(&users[username])));
    }

    else {
        // send message utente non registrato
        //ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        emit errorMessage(socket, "Client not registered.");
    }
}

void Tasks::serverLoginUnlock()
{
    qDebug() << "provo a completare";

    ServerDatabase database;
    database.open(defaultnamedb,threadId,nullptr);

    QString token = request.value("challange").toString();
    QSharedPointer<Client> client = clients[socket];

    QByteArray data;

    if (client->isLogged()){
        emit errorMessage(socket, "You have to loggin before use the platform");
        return ;

    }else if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        //ui->commet->appendPlainText( "User " + client->getUsername() + " is logged in");
        client->login(client->getUser());
        emit messageChallegePassed(socket, QString());
        return ;
    }else{
        users.remove(client->getUsername());
        client->logout();
        emit errorMessage(socket, "Credential inserted are not corrected");
        return ;
    }
}

void Tasks::test(QJsonObject m){
    qDebug() << m["username"].toString() << QThread::currentThreadId();
}

void Tasks::run(){
    switch (typeOp) {
        //case AccountCreate: this->serverAccountCreate(clientSocket,request);break;
        case TypeOperation::LoginRequest: this->serverLoginRequest();break;
        case TypeOperation::LoginUnlock: this->serverLoginUnlock();break;
        default: qDebug() << "No Task";
    }
}

Tasks::~Tasks()
{
    //this->database.removeDatabase(threadId);
    qDebug() << "Return to main thread  " << QCoreApplication::instance()->thread();
    QSqlDatabase::removeDatabase(threadId);
}
