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
    this->po=ProcessOperation::getInstance(this, clients, users);
    
    //opening database;
    try {
        if(!SSLconfiguration(sslconfig))
            throw StartupException("Impossible to set encryption configuration");
        m_pWebSocketServer->setSslConfiguration(sslconfig);

        database.open(defaultnamedb, QString("dafault"), ui.get());

    }catch ( DatabaseConnectionException& dce ) {
        throw StartupException("Connection database error");
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
    // Possible implementation , load at the begining in map

    for (UserData user : database.readUsersList() )
    {
        for (QString docUri : database.readUserDocuments(user.getUsername()))
        {
            user.addDocument(docUri);
        }
        users.insert(user.getUsername(), user);
    }

    // Initialize the counter

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

    qDebug() << pSocket;

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

void MainWindow::processBinaryMessage(QByteArray message)
{
    qDebug() << "new binary";
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

    try {
        ui->commet->appendPlainText("Contenuto json: "+jsonDoc.toJson());

        /*
        if(po->checkTypeOperationGranted((TypeOperation)mType).isNull() ||
                po->checkTypeOperationGranted((TypeOperation)mType).isEmpty()){
            ui->commet->appendPlainText("***(MESSAGE ERROR)*** Received unexpected message: ");
            // return whitout do anything.
            return;
        }
        */

        // QUi dispatch nel pool
        po->process(socket, jsonObj );
    }
    catch (std::exception& me)
    {
        ui->commet->appendPlainText( me.what());
        socketAbort(socket);
    }
}
