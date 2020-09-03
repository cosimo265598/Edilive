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
#include "../Client/user.h"
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

    ui->commet->appendPlainText("(INIT COMPLETE ) users counter: "+ QString::number(users.count()) );
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

void MainWindow::serverErrorConnection(QWebSocketProtocol::CloseCode closeCode)
{
    ui->commet->appendPlainText("Errors occurred during ssetup socket\n"+QString(closeCode));
    // alert message should be built
}



void MainWindow::on_startserver_2_clicked()
{
    ui->commet->clear();
}

void MainWindow::SimpleTextMessageTest(){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSIGNAL TEST MESSAGE FROM CLIENT RECEIVED TO SERVER");
}

/*
 *  LOGIN METHOD
*/
void MainWindow::serverLoginRequest(QWebSocket* clientSocket, QString username){
    QSharedPointer<Client> client = clients[clientSocket];

    if (users.contains(username))
    {
        if (client->isLogged()){
            ui->commet->appendPlainText("Client already logged in as '" + client->getUsername() + "'");
            clientSocket->sendBinaryMessage(BuilderMessage::MessageLoginError("Client: "+client->getUsername()+" is already logged in.").toJson());
            return;
        }
        // client not logged
        clientSocket->sendBinaryMessage(BuilderMessage::MessageChallege(
        QString(users[username].getSalt()) ,
        QString(client->challenge(&users[username])) ).toJson());

    }
    else {
        // send message utente non registrato
        ui->commet->appendPlainText("Client not Registered : ' " + client->getUsername() + "'");
        clientSocket->sendBinaryMessage(BuilderMessage::MessageLoginError("Client not registered.").toJson());
    }

}

void MainWindow::serverLoginUnlock(QWebSocket *clientSocket, QString token)
{
    QSharedPointer<Client> client = clients[clientSocket];

    if (client->isLogged()){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageLoginError("You have to loggin before use the platform").toJson());
        return ;
    }
    if (client->authenticate(token.toUtf8()))		// verify the user's account credentials
    {
        ui->commet->appendPlainText( "User " + client->getUsername() + " is logged in");
        client->login(client->getUser());
        clientSocket->sendBinaryMessage(BuilderMessage::MessageChallegePassed(QString()).toJson());
        return ;
    }
    else
    {
        client->logout();
        clientSocket->sendBinaryMessage(BuilderMessage::MessageLoginError("Credential inserted are not corrected").toJson());
        return ;
    }


}

void MainWindow::serverAccountCreate(QWebSocket *clientSocket, QString username, QString nickname, QImage icon, QString password)
{
    QSharedPointer<Client> client = clients[clientSocket];
    if (client->isLogged()){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("Client already logged in").toJson());
        return;
    }
    /* check if username or password are nulls */
    if (!username.compare("") || !password.compare("")){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("User/Psw can not be empty").toJson());
        return;
    }
    /* check username length */
    if (username.length() > MAX_NAME_LENGTH){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("Username too long").toJson());
        return;
    }
    /* check if this username is already used */
    if (users.contains(username)){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("Username already exist").toJson());
        return;
    }
    /* check whitespaces */
    if (!QRegExp("^[^\\s]+$").exactMatch(username)){
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("Username mut not be only whitespaces").toJson());
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
    }
    catch (QException& dbe) {
        ui->commet->appendPlainText(dbe.what());
        client->logout();
        users.remove(username);
        clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountError("Internal Error").toJson());
        return;
    }

    clientSocket->sendBinaryMessage(BuilderMessage::MessageAccountConfirmed("Account Created Correctly").toJson());
    return;

}

void MainWindow::OpenDirOfClient(QWebSocket *clientSocket)
{
    QSharedPointer<Client> client = clients[clientSocket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername());

    QDirIterator it(path, QDir::Files, QDirIterator::NoIteratorFlags);
    QJsonArray files;
    // visita della directory
    while (it.hasNext()) {
        it.next();
        files.append(QJsonObject{
                         {"filename", it.fileInfo().fileName()}, // meglio file name
                         {"owner", it.fileInfo().owner()},
                         {"lastModified",  it.fileInfo().lastModified().toString()},
                         {"lastRead", it.fileInfo().lastRead().toString()}
                     });
    }
    //qDebug()<<BuilderMessage::MessageOpenDirOfClient(files).toJson(QJsonDocument::Indented);
    clientSocket->sendBinaryMessage(BuilderMessage::MessageOpenDirOfClient(files).toJson(QJsonDocument::Indented));
}

void MainWindow::CreateFileForClient(QWebSocket *clientSocket, QString file)
{
    qDebug()<<"segnale nuovo file ricevuto";
    QSharedPointer<Client> client = clients[clientSocket];

    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+file);

    QFile filecreate(path);
    if(filecreate.exists()){
        qDebug()<< " File già presente- "<<file;
        clientSocket->sendBinaryMessage(BuilderMessage::MessageFileClientError
                                        ("La tua directory contine già un file con questo nome.")
                                        .toJson());
        return;
    }
    if (filecreate.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&filecreate);
        out << "";
        filecreate.close();
        OpenDirOfClient(clientSocket);
    }else
    {
        clientSocket->sendBinaryMessage(BuilderMessage::MessageFileClientError
                                        ("Impossibile creare il file. Riprova.")
                                        .toJson());
    }
}

void MainWindow::processBinaryMessage(QByteArray message)
{
    // processare il messoggio , leggendo i primi 16 bit per identificare il tipo di messaggio sul socket
    // dopo chimare la funzione processmessage
    // che chiamera una funzione dentro questa classe per la sua elaborazione
    QWebSocket* socket = dynamic_cast<QWebSocket*>(sender());
    if (socket == nullptr || !socket->isValid())
        return;

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message, &parseError);

    // Fail if the JSON is invalid.
    if (parseError.error != QJsonParseError::NoError)
        return ;
    // Make sure the root is an object.
    if (!jsonDoc.isObject())
        return ;

    QJsonObject root = jsonDoc.object();
    quint16 mType = static_cast<quint16>(root.value("type").toInt());
/*
    QDataStream streamIn(&message, QIODevice::ReadOnly);	// connect stream with socket
    streamIn.setVersion( QDataStream::Qt_5_14);
    quint16 mType;
    streamIn>>mType;
*/


    try {
        ui->commet->appendPlainText("Contenuto json: "+jsonDoc.toJson());

        ProcessOperation po(this);

        if (mType == LoginRequest || mType == LoginUnlock || mType == AccountCreate ||
            mType == AccountUpdate || mType == Logout || mType == Simplemessage
                || mType == OpenDirectory || mType== CreateFile || mType== OpenFile)
        {
            po.process((TypeOperation)mType, socket, root );
        }
        else
            ui->commet->appendPlainText("(MESSAGE ERROR) Received unexpected message: ");

    }
    catch (std::exception& me)
    {
        ui->commet->appendPlainText( me.what());
    }
}

