#include "tasks.h"

Tasks::Tasks(QObject *parent) :
    QObject(parent)
{
}

void Tasks::serverAccountCreate(QWebSocket *clientSocket, QString username, QString password, QMap<QWebSocket*, QSharedPointer<Client>>& clients, QMap<QString, UserData>& users, ServerDatabase& database)
{

    clientSocket->moveToThread(QThread::currentThread());
    QByteArray data;

    //check if this username is already used
    try {
        UserData user(database.readUser(username));
        if(!user.isEmpty()){
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageAccountError("Username already exist"));
            clientSocket->sendBinaryMessage(data);
            return;
        }
    }  catch (QException& re) {
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
    users.insert(username, user);               /* insert new user in map	*/
    qDebug() << users[username].getNickname() << " " << users[username].getUserId() << " " ;

    try
    {	// Add the new user record to the server database
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
       // ui->commet->appendPlainText(dbe.what());
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

void Tasks::doWork(){

}
