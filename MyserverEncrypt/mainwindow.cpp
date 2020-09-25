#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtWebSockets/QWebSocketServer"
#include "QtWebSockets/QWebSocket"
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>
#include <QDebug>
#include <QString>
#include <iostream>
#include "buildermessage.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow),
    m_pWebSocketServer(nullptr)
{
    ui->setupUi(this);

   // Setting visibility button
    ui->stopserver->setEnabled(false);
    ui->commet->setReadOnly(true);
    ui->commetdb->setReadOnly(true);

    // config
    m_pWebSocketServer =  QSharedPointer<QWebSocketServer>(new QWebSocketServer("SSL_Server",QWebSocketServer::SecureMode,this));
    SSLconfiguration(sslconfig);
    m_pWebSocketServer->setSslConfiguration(sslconfig);

    //opening database;
    database.open(defaultnamedb,ui.get());
}

MainWindow::~MainWindow()
{
    m_pWebSocketServer->close();
}

void MainWindow::prepareToStart()
{
    ui->commet->appendPlainText("(INIT STARTED)" );

    //UserData userprova("hello",1,"prova","ciao",QImage());
    //UserData userprova2("good",2,"prova","tutto",QImage());
    //database.insertUser(userprova);
    //database.insertUser(userprova2);
    // Check existence of the required SSL certificate files
    if (!QFile("localhost.key").exists()) {
        ui->commet->appendPlainText("Not find localhost.key - Server can not start!");
        //throw StartupException("Cannot find private key file: 'server.key'");
    }
    if (!QFile("localhost.cert").exists()) {
        ui->commet->appendPlainText("Not find localhost.cert - Server can not start!");
        //throw StartupException("Cannot find local certificate file: 'server.pem'");
    }

    // Check existence of (or create) the Users folder
    if (!QDir("Users").exists())
    {
        ui->commet->appendPlainText("Creating the server Users folder");
        if (!QDir().mkdir("Users")) {
            ui->commet->appendPlainText("Cannot create folder '.\\Users'");
        }
    }

    // Read The database file , avoid to interview each time the db
    for (UserData user : database.readUsersList() )
    {
        for (QString docUri : database.readUserDocuments(user.getUsername()))
        {
            user.addDocument(docUri);
        }
        users.insert(user.getUsername(), user);
    }
    // Initialize the counter
    userId = database.getMaxUserID();

    ui->commet->appendPlainText("Users counter: "+ QString::number(users.count()) );

    ui->commet->appendPlainText("(INIT COMPLETE )" );
}


void MainWindow::SSLconfiguration(QSslConfiguration& sslConfiguration){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSetting configuration SSL ...");

    QFile certFile(QStringLiteral(":/localhost.cert"));
    QFile keyFile(QStringLiteral(":/localhost.key"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tConfiguration SSL finished... ");


}

void MainWindow::on_startserver_clicked()
{

    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tTry to start Server ...");

    if (!m_pWebSocketServer->listen(QHostAddress::LocalHost,qint64(ui->nport->value()))) {
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tServer is already started on this port. Try to change num port.");
    }else {
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tServer started...");
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tIP server:\t"+m_pWebSocketServer->serverAddress().toString()+" on port:\t"+QString::number(m_pWebSocketServer->serverPort()));
        ui->stopserver->setEnabled(true);
        ui->startserver->setEnabled(false);
        connect(m_pWebSocketServer.get(), &QWebSocketServer::newConnection,this, &MainWindow::onNewConnection);
        connect(m_pWebSocketServer.get(), &QWebSocketServer::sslErrors,this, &MainWindow::onSslErrors);
        connect(m_pWebSocketServer.get(), &QWebSocketServer::serverError,this, &MainWindow::serverErrorConnection);
        return;
    }
}

void MainWindow::on_stopserver_clicked()
{
    if(m_pWebSocketServer->isListening()){
        m_pWebSocketServer->close();

/*
        QMap<QWebSocket*, QSharedPointer<Client>>::const_iterator i = clients.constBegin();
        while ( i != clients.constEnd() ) {
            i.value()->logout();
            i.key()->close();
            i.key()->deleteLater();
            ++i;
        }
*/
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tAll client will be disconnected...");
        ui->stopserver->setEnabled(false);
        ui->startserver->setEnabled(true);
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tServer stopped...");

    }
}

void MainWindow::onNewConnection(){

    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    if(pSocket==nullptr || !pSocket->isValid())
        return;
    
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient connected: "+pSocket->peerName()+"IP: "+pSocket->peerAddress().toString());

    //connect(pSocket, &QWebSocket::textMessageReceived, this, &MainWindow::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived,this, &MainWindow::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &MainWindow::socketDisconnected);


    // Create a new client object
    QSharedPointer<Client> client(new Client(pSocket));
    clients.insert(pSocket, client);

    ui->num_client->display(ui->num_client->intValue()+1);

}

void MainWindow::processTextMessage(QString message)
{
    QWebSocket *socket = qobject_cast<QWebSocket *>(sender());
    if (socket == nullptr || !socket->isValid())
        return;

    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient ha inviato:\t"+message);

}

void MainWindow::socketDisconnected()
{
    QWebSocket* socket = dynamic_cast<QWebSocket*>(sender());
    QSharedPointer<Client> client = clients[socket];

    if (client->isLogged())
    {
        client->logout();
    }

    clients.remove(socket);					/* remove this client from the map */
    socket->close();						/* close and destroy the socket */
    socket->deleteLater();

    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient "+client->getUsername()+" disconnected");
    ui->num_client->display(ui->num_client->intValue()-1);

}

void MainWindow::onSslErrors(const QList<QSslError> & sslerror)
{
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSsl errors occurred");
    for(auto e : sslerror){
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tError"+e.errorString());
    }
}

void MainWindow::socketAbort(QWebSocket* clientSocket)
{
    // Disconnect all the socket's signals from server slots
    disconnect(clientSocket, &QWebSocket::binaryMessageReceived, this, &MainWindow::processBinaryMessage);
    disconnect(clientSocket, &QWebSocket::disconnected, this, &MainWindow::socketDisconnected);

    QSharedPointer<Client> client = clients[clientSocket];

    clientSocket->abort();						/* abort and destroy the socket */
    clientSocket->deleteLater();

    clients.remove(clientSocket);				/* remove this client from the active connections */
    if (client->isLogged())
    {
        client->logout();
        ui->commet->appendPlainText("Eject "+client->getUsername()+" ip: "+clientSocket->peerAddress().toString());
    }
    else
        ui->commet->appendPlainText("Shutdown connection to unidentified client");
}

void MainWindow::serverErrorConnection(QWebSocketProtocol::CloseCode closeCode)
{
    ui->commet->appendPlainText("Errors occurred during ssetup socket\n"+QString(closeCode));
    // alert message should be built
}



void MainWindow::on_startserver_2_clicked() {  ui->commet->clear(); }

void MainWindow::SimpleTextMessageTest(){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSIGNAL TEST MESSAGE FROM CLIENT RECEIVED TO SERVER");
}

/*
 *  LOGIN METHOD
*/
void MainWindow::serverLoginRequest(QWebSocket* clientSocket, QString username){
    QSharedPointer<Client> client = clients[clientSocket];

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    if (users.contains(username))
    {
        if (client->isLogged()){
            ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
            stream << BuilderMessage::MessageLoginError("Client: "+client->getUsername()+" is already logged in.");
            clientSocket->sendBinaryMessage(data);
            return;
        }
        // client not logged

        stream << BuilderMessage::MessageChallege(
                      QString(users[username].getSalt()) ,
                      QString(client->challenge(&users[username])));

        clientSocket->sendBinaryMessage(data);

    }
    else {
        // send message utente non registrato
        ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        stream << BuilderMessage::MessageLoginError("Client not registered.");
        clientSocket->sendBinaryMessage(data);
    }
}

void MainWindow::serverLoginUnlock(QWebSocket *clientSocket, QString token)
{
    QSharedPointer<Client> client = clients[clientSocket];

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    if (client->isLogged()){
        stream << BuilderMessage::MessageLoginError("You have to loggin before use the platform");
        clientSocket->sendBinaryMessage(data);
        return ;
    }else if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        ui->commet->appendPlainText( "User " + client->getUsername() + " is logged in");
        client->login(client->getUser());
        stream << BuilderMessage::MessageChallegePassed(QString());
        clientSocket->sendBinaryMessage(data);
        return ;
    }else{
        client->logout();
        stream << BuilderMessage::MessageLoginError("Credential inserted are not corrected");
        clientSocket->sendBinaryMessage(data);
        return ;
    }
}

void MainWindow::serverAccountCreate(QWebSocket *clientSocket, QString username, QString nickname, QImage icon, QString password)
{
    QSharedPointer<Client> client = clients[clientSocket];
    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    if (client->isLogged()){
        stream << BuilderMessage::MessageAccountError("Client already logged in");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    /* check if username or password are nulls */
    if (!username.compare("") || !password.compare("")){
        stream << BuilderMessage::MessageAccountError("User/Psw can not be empty");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    /* check username length */
    if (username.length() > MAX_NAME_LENGTH){
        stream << BuilderMessage::MessageAccountError("Username too long");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    /* check if this username is already used */
    if (users.contains(username)){
        stream << BuilderMessage::MessageAccountError("Username already exist");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    /* check whitespaces */
    if (!QRegExp("^[^\\s]+$").exactMatch(username)){
        stream << BuilderMessage::MessageAccountError("Username mut not be only whitespaces");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    // no check on image because are used for the firts time default settings

    ui->commet->appendPlainText("Creating new user account "+username);

    UserData user(username, userId++, nickname, password, icon);		/* create a new user		*/
    users.insert(username, user);               /* insert new user in map	*/

    client->login(&users[username]);			// client is automatically logged in as the new user

    try
    {	// Add the new user record to the server database
        database.insertUser(user);
        if (!QDir(QDir().currentPath()+"/Users").mkdir(client->getUsername())) {
            ui->commet->appendPlainText("Cannot create folder for Users"+ client->getUsername());
        }
        // inserimento di un file di prova
        QFile::copy(QDir().currentPath()+"/example.html",
                    QDir().currentPath()+"/Users/"+client->getUsername()+"/examplefile");

    }
    catch (QException& dbe) {
        ui->commet->appendPlainText(dbe.what());
        client->logout();
        users.remove(username);
        QDir(QDir().currentPath()+"/Users").rmdir(client->getUsername());
        stream << BuilderMessage::MessageAccountError("Internal Error");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    stream << BuilderMessage::MessageAccountConfirmed("Account Created Correctly");
    clientSocket->sendBinaryMessage(data);
    return;

}

void MainWindow::OpenDirOfClient(QWebSocket *clientSocket)
{
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername());
    QDirIterator it(path, QDir::Files, QDirIterator::NoIteratorFlags);
    QJsonArray files;

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    // visita della directory
    while (it.hasNext()) {
        it.next();
        files.append(QJsonObject{
                         {"filename", it.fileInfo().fileName()}, // meglio file name
                         {"owner", it.fileInfo().owner()},
                         {"lastModified",  it.fileInfo().lastModified().toString()},
                         {"lastRead", it.fileInfo().lastRead().toString()},
                         {"size", QString::number(it.fileInfo().size()) }
                     });
    }
    stream << BuilderMessage::MessageOpenDirOfClient(files);
    clientSocket->sendBinaryMessage(data);
}

void MainWindow::CreateFileForClient(QWebSocket *clientSocket, QString file)
{
    qDebug()<<"segnale nuovo file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+file);

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    QFile filecreate(path);
    if(filecreate.exists()){
        qDebug()<< " File già presente- "<<file;
        stream << BuilderMessage::MessageFileClientError
                  ("La tua directory contine già un file con questo nome.");
        clientSocket->sendBinaryMessage(data);
        return;
    }
    if (filecreate.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&filecreate);
        out << "";
        filecreate.close();
        OpenDirOfClient(clientSocket);
    }else{
        stream << BuilderMessage::MessageFileClientError
                  ("Impossibile creare il file. Riprova.");
        clientSocket->sendBinaryMessage(data);
    }
}

void MainWindow::OpenFileForClient(QWebSocket *clientSocket, QString fileName)
{
    qDebug()<<"Segnale apertura file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    stream << BuilderMessage::MessageHeaderFile(fileName);

    QFile filecreate(path);
    if(filecreate.exists()){
        if (filecreate.open(QIODevice::ReadWrite)){
            stream << filecreate.readAll();
            filecreate.close();
        }
    }
    else
        return;

    clientSocket->sendBinaryMessage(data);
}

void MainWindow::PersonalDataOfClient(QWebSocket *clientSocket)
{
    qDebug()<<"segnale di aperura dati profilo ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];

    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_14);

    //debug invio immagine di prova
    QImage img(QDir().currentPath()+"/logo32.png");

    stream<< BuilderMessage::MessageProfileData(
                 users[client->getUsername()].getUsername(),
                 users[client->getUsername()].getNickname(),
                 /*users[client->getUsername()].getIcon()*/img);

    clientSocket->sendBinaryMessage(data);
    qDebug()<<data;
}


void MainWindow::processBinaryMessage(QByteArray message)
{
    QWebSocket* socket = dynamic_cast<QWebSocket*>(sender());
    if (socket == nullptr || !socket->isValid())
        return;

    QDataStream stream (&message, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_14);
    QJsonDocument jsonDoc;
    stream >> jsonDoc;

    // Fail if the JSON is invalid.
    if (jsonDoc.isNull()){
        qDebug() << "JSON NULL";
         return ;
    }
    // Make sure the root is an object.
    if (!jsonDoc.isObject()){
         qDebug() << "JSON not a obj";
          return ;
    }

    QJsonObject jsonObj = jsonDoc.object();
    int mType = jsonObj["type"].toInt();

    this->po=ProcessOperation::getInstance(this);

       try {
           ui->commet->appendPlainText("Contenuto json: "+jsonDoc.toJson());

           if(po->checkTypeOperationGranted((TypeOperation)mType).isNull() ||
                   po->checkTypeOperationGranted((TypeOperation)mType).isEmpty()){
               ui->commet->appendPlainText("***(MESSAGE ERROR)*** Received unexpected message: ");
               // return whitout do anything.
               return;
           }

           po->process((TypeOperation)mType, socket, jsonObj );

           /*
           if (mType == LoginRequest || mType == LoginUnlock || mType == AccountCreate ||
               mType == AccountUpdate || mType == Logout || mType == Simplemessage
                   || mType == OpenDirectory || mType== CreateFile || mType== OpenFile || mType==ProfileData)
           {
               po->process((TypeOperation)mType, socket, jsonObj );
           }
           else
               ui->commet->appendPlainText("(MESSAGE ERROR) Received unexpected message: ");
           */
       }
       catch (std::exception& me)
       {
           ui->commet->appendPlainText( me.what());
           socketAbort(socket);
       }}

