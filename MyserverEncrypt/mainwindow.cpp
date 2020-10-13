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
#include "serverexception.h"

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
    this->po=ProcessOperation::getInstance(this, database, clients, users);

    //thread poll config
    pool = new QThreadPool(this);
    pool->setMaxThreadCount(std::thread::hardware_concurrency());
    
    //opening database;
    try {
        if(!SSLconfiguration(sslconfig))
            throw StartupException("Impossible to set encryption configuration");
        m_pWebSocketServer->setSslConfiguration(sslconfig);
        database.open(defaultnamedb,ui.get());
    }catch ( DatabaseConnectionException& dce ) {
        throw StartupException("Connetion database error");
    }catch(DatabaseCreateException& dcreate_ex){
        throw StartupException("Creation database table error");
    }catch(StartupException& se){
        throw StartupException("Start up problem.");
    }
}

MainWindow::~MainWindow()
{
    m_pWebSocketServer->close();
}

void MainWindow::prepareToStart()
{
    ui->commet->appendPlainText("(INIT STARTED)" );

/*    UserData d(database.readUser("ntipon"));
    if(d.isEmpty())
        qDebug()<<"true";
    qDebug()<<d.getSalt()<<d.getNickname();*/

    // Check existence of the required SSL certificate files
    if (!QFile("localhost.key").exists()) {
        ui->commet->appendPlainText("Not find localhost.key - Server can not start!");
        throw StartupException("Cannot find private key file");
    }
    if (!QFile("localhost.cert").exists()) {
        ui->commet->appendPlainText("Not find localhost.cert - Server can not start!");
        throw StartupException("Cannot find local certificate file");
    }

    // Check existence of (or create) the Users folder
    if (!QDir("Users").exists())
    {
        ui->commet->appendPlainText("Creating the server Users folder");
        if (!QDir().mkdir("Users")) {
            ui->commet->appendPlainText("Cannot create folder '.\\Users'");
            throw StartupException("Cannot create folder '.\\Users'");

        }
    }

    // Read The database file , avoid to interview each time the db
    QList<UserData> listUser=database.readUsersList();
    /* Possible implementation , load at the begining in map

    for (UserData user : database.readUsersList() )
    {
        for (QString docUri : database.readUserDocuments(user.getUsername()))
        {
            user.addDocument(docUri);
        }
        users.insert(user.getUsername(), user);
    }*/

    // Initialize the counter
    userId = database.getMaxUserID();

    ui->commet->appendPlainText("Users in database: "+ QString::number(listUser.count()) );

    ui->commet->appendPlainText("(INIT COMPLETE )" );
}


bool MainWindow::SSLconfiguration(QSslConfiguration& sslConfiguration){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSetting configuration SSL ...");

    QFile certFile(QStringLiteral(":/localhost.cert"));
    QFile keyFile(QStringLiteral(":/localhost.key"));
    if ( !certFile.open(QIODevice::ReadOnly) ||  !keyFile.open(QIODevice::ReadOnly) )
        return false;
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tConfiguration SSL finished... ");
    return true;
}

void MainWindow::on_startserver_clicked()
{

    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tTry to start Server ...");

    if (!m_pWebSocketServer->listen(QHostAddress::LocalHost,qint64(ui->nport->value()))) {
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tServer is busy on this port. Try to change num port.");
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

        // utile se viene mandato un messaggio di caduta server a tutti i client
        for(auto& element : clients.toStdMap()){
            element.first->close();
            element.first->deleteLater();
            element.second->logout();
        }
        clients.clear();

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
    for(auto& e : sslerror){
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tError"+e.errorString());
    }
}

void MainWindow::socketAbort(QWebSocket* clientSocket)
{
    // Disconnect all the socket's signals from server slots
    disconnect(clientSocket, &QWebSocket::binaryMessageReceived, this, &MainWindow::processBinaryMessage);
    disconnect(clientSocket, &QWebSocket::disconnected, this, &MainWindow::socketDisconnected);

    QSharedPointer<Client> client = clients[clientSocket];
    QString ip=clientSocket->peerAddress().toString();

    clientSocket->abort();						/* abort and destroy the socket */
    clientSocket->deleteLater();

    clients.remove(clientSocket);				/* remove this client from the active connections */
    if (client->isLogged())
    {
        client->logout();
        ui->commet->appendPlainText("Eject "+client->getUsername()+" ip: "+ip);
    }
    else
        ui->commet->appendPlainText("Shutdown connection client: "+ip);
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

    try {

        UserData user(database.readUser(username));

        if(user.isEmpty()){            // utente non registrato, non presente nel db.
            ui->commet->appendPlainText("Client not Registered "+username);
            BuilderMessage::MessageSendToClient(
                        data,
                        BuilderMessage::MessageLoginError("Client not registered."));

            clientSocket->sendBinaryMessage(data);
            socketAbort(clientSocket);



        }else{
            for (QString docUri : database.readUserDocuments(user.getUsername()))
                user.addDocument(docUri);
            users.insert(user.getUsername(), user);
        }
    }  catch (DatabaseReadException& re) {
        users.remove(username);
        ui->commet->appendPlainText("Databaseread problem during the query excution");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Error database sever"));
        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
        return;
    }


    if (users.contains(username))
    {
        if (client->isLogged()){
            ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
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

void MainWindow::serverLoginUnlock(QWebSocket *clientSocket, QString token)
{
    QSharedPointer<Client> client = clients[clientSocket];


    QByteArray data;

    if (client->isLogged()){
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageLoginError("You have to loggin before use the platform"));
        clientSocket->sendBinaryMessage(data);
        return ;
    }else if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        ui->commet->appendPlainText( "User " + client->getUsername() + " is logged in");
        client->login(client->getUser());
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageChallegePassed(QString()));
        clientSocket->sendBinaryMessage(data);
        return ;
    }else{
        client->logout();
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageLoginError("Credential inserted are not corrected"));
        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
        users.remove(client->getUsername());
        return ;
    }
}

void MainWindow::serverAccountCreate(QWebSocket *clientSocket, QString username, QString password)
{
    QSharedPointer<Client> client = clients[clientSocket];
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
    }  catch (DatabaseReadException& re) {
        ui->commet->appendPlainText("Databaseread problem during the query execution");
        BuilderMessage::MessageSendToClient(
                    data,
                    BuilderMessage::MessageLoginError("Error database sever during registration phase"));
        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
        return;
    }
    // no check on image because are used for the firts time default settings

    ui->commet->appendPlainText("Creating new user account "+username);
    UserData user(username, userId++, "nickname", password, QImage(":/images/default.png"));		/* create a new user		*/
    users.insert(username, user);               /* insert new user in map	*/
    qDebug() << users[username].getNickname() << " " << users[username].getUserId() << " " ;

    try
    {	// Add the new user record to the server database
        database.insertUser(user);
        if (!QDir(QDir().currentPath()+"/Users").mkdir(username)) {
            ui->commet->appendPlainText("Cannot create folder for Users: "+ client->getUsername());
            throw DatabaseCreateException("Can not create folder for new user",QSqlError());
        }
        // inserimento di un file di prova
        bool state=QFile::copy(QDir().currentPath()+"/example.html",
                    QDir().currentPath()+"/Users/"+username+"/examplefile");
        if(state)
            throw DatabaseCreateException("Can not copy file example for new user",QSqlError());

    }catch (DatabaseException& dbe) {
        ui->commet->appendPlainText(dbe.what());
        client->logout();
        users.remove(username);
        QDir(QDir().currentPath()+"/Users").rmdir(client->getUsername());
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageAccountError("Internal Error"));
        clientSocket->sendBinaryMessage(data);
        socketAbort(clientSocket);
        return;
    }
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed("Account Created Correctly"));
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
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageOpenDirOfClient(files));
    clientSocket->sendBinaryMessage(data);
}

void MainWindow::CreateFileForClient(QWebSocket *clientSocket, QString file)
{
    qDebug()<<"segnale nuovo file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+file);

    QByteArray data;

    QFile filecreate(path);
    if(filecreate.exists()){
        qDebug()<< " File già presente- "<<file;
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageFileCreationError("File already exists"));
        clientSocket->sendBinaryMessage(data);
        return;
    }
    if (filecreate.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&filecreate);
        out << "";
        filecreate.close();
        //fileCreationSuccess(clientSocket, path);
        OpenDirOfClient(clientSocket);

    }else{
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageFileCreationError("Error creation the new file."));
        clientSocket->sendBinaryMessage(data);
    }
}

void MainWindow::OpenFileForClient(QWebSocket *clientSocket, QString fileName)
{
    qDebug()<<"Segnale apertura file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QByteArray data;
    BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageHeaderFile(fileName));

    QFile filecreate(path);
    if(filecreate.exists()){
        if (filecreate.open(QIODevice::ReadWrite)){
            QByteArray contentFile(filecreate.readAll());
            BuilderMessage::MessageSendToClient(data,contentFile);
            filecreate.close();
        }
    }
    else
        return;

    clientSocket->sendBinaryMessage(data);
}

void MainWindow::DeleteFileForClient(QWebSocket *clientSocket, QString fileName)
{
    qDebug()<<"segnale delete file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QByteArray data;

    QFile file(path);
    if(!file.exists()){
        qDebug()<< "File not found";
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageFileDeletionError("File not found"));
        clientSocket->sendBinaryMessage(data);
        return;
    }else{
        file.remove();
        OpenDirOfClient(clientSocket);
    }
}

void MainWindow::PersonalDataOfClient(QWebSocket *clientSocket)
{
    qDebug()<<"segnale di apertura dati profilo ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];

    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountInfo(
                users[client->getUsername()].getUsername(),
                users[client->getUsername()].getNickname(),
                users[client->getUsername()].getIcon()));

    clientSocket->sendBinaryMessage(data);
    //qDebug()<<data;
}

void MainWindow::updateProfileClient(QWebSocket *clientSocket, QString nickname, QString password, QString stringifiedImage ){
    qDebug()<<"segnale di modifica dati profilo ricevuto";

    QSharedPointer<Client> client = clients[clientSocket];
    QByteArray data;
    QImage image;
    image.loadFromData(QByteArray::fromBase64(stringifiedImage.toLatin1()),"PNG");

    users[client->getUsername()].update(nickname, password, image);

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateSuccess("Update success"));

    clientSocket->sendBinaryMessage(data);
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

    socket->flush();

    try {
        ui->commet->appendPlainText("Contenuto json: "+jsonDoc.toJson());

        if(po->checkTypeOperationGranted((TypeOperation)mType).isNull() ||
                po->checkTypeOperationGranted((TypeOperation)mType).isEmpty()){
            ui->commet->appendPlainText("***(MESSAGE ERROR)*** Received unexpected message: ");
            // return whitout do anything.
            return;
        }

        // QUi dispatch nel pool

        QThreadPool::globalInstance()->start(po->process((TypeOperation)mType, socket, jsonObj));

        //po->process((TypeOperation)mType, socket, jsonObj );


    }
    catch (std::exception& me)
    {
        ui->commet->appendPlainText( me.what());
        socketAbort(socket);
    }
}
