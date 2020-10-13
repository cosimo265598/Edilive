#include "tasks.h"

Tasks::Tasks(QObject *parent, QWebSocket *clientSocket, QJsonObject request, ServerDatabase database, QMap<QWebSocket*, QSharedPointer<Client>> clients, QMap<QString, UserData> users, TypeOperation type) :
    QObject(parent),
    database(database),
    clients(clients),
    users(users),
    type(type),
    clientSocket(clientSocket),
    request(request)
{
    qDebug() << "creato";
    clientSocket->moveToThread(QThread::currentThread());
}

void Tasks::serverAccountCreate(QWebSocket *clientSocket, QJsonObject request)
{
    QString username = request["username"].toString();
    QString password = request["password"].toString();

    QSharedPointer<Client> client = clients[clientSocket];
    QByteArray data;

    qDebug() << client->getUserId();

    //check if this username is already used
    try {
        qDebug() << "try read user";
        UserData user(database.readUser(username));
        if(!user.isEmpty()){
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageAccountError("Username already exist"));
            clientSocket->sendBinaryMessage(data);
            return;
        }
    }  catch (DatabaseReadException& re) {
        //ui->commet->appendPlainText("Databaseread problem during the query execution");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Error database sever during registration phase"));
        clientSocket->sendBinaryMessage(data);
        //socketAbort(clientSocket);
        return;
    }
    // no check on image because are used for the firts time default settings

    //ui->commet->appendPlainText("Creating new user account "+username);
    UserData user(username, users.size()+1, "nickname", password, QImage(":/images/default.png"));		/* create a new user		*/
    qDebug() << "try create user";
    users.insert(username, user);               /* insert new user in map	*/
    qDebug() << users.value(username).getNickname() << " " << users.value(username).getUserId() << " " ;

    try
    {	// Add the new user record to the server database
        qDebug() << "try add user to db";
        database.insertUser(user);
        if (!QDir(QDir().currentPath()+"/Users").mkdir(username)) {
            //ui->commet->appendPlainText("Cannot create folder for Users: "+ client->getUsername());
            throw DatabaseCreateException("Can not create folder for new user",QSqlError());
        }
        // inserimento di un file di prova
        bool state=QFile::copy(QDir().currentPath()+"/example.html",
                    QDir().currentPath()+"/Users/"+username+"/examplefile");
        if(state)
            throw DatabaseCreateException("Can not copy file example for new user",QSqlError());

    }catch (DatabaseException& dbe) {
        //ui->commet->appendPlainText(dbe.what());
        client->logout();
        users.remove(username);
        QDir(QDir().currentPath()+"/Users").rmdir(client->getUsername());
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageAccountError("Internal Error"));
        clientSocket->sendBinaryMessage(data);
        //socketAbort(clientSocket);
        return;
    }
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed("Account Created Correctly"));
    clientSocket->sendBinaryMessage(data);
    return;

}

void Tasks::serverLoginRequest(QWebSocket* clientSocket, QJsonObject request){

    QString username = request["username"].toString();

    QSharedPointer<Client> client = clients[clientSocket];
    QByteArray data;

    try {

        qDebug() << "In Try scope   " << client->getUserId();
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
    /*
    else {
        // send message utente non registrato
        ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Client not registered."));

        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
    }*/
}


void Tasks::test(QJsonObject m){
    qDebug() << m["username"].toString() << QThread::currentThreadId();
}

void Tasks::run(){
    switch (type) {
        case AccountCreate: this->serverAccountCreate(clientSocket,request);break;
        case LoginRequest: this->serverLoginRequest(clientSocket,request);break;
        default: qDebug() << "No Task";

    }
}
