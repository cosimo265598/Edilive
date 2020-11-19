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
}

MainWindow::~MainWindow()
{
    m_pWebSocketServer->close();
}

void MainWindow::prepareToStart()
{
    ui->commet->appendPlainText("(INIT STARTED)" );

    // Check existence of the required SSL certificate files
    if (!QFile("localhost.key").exists()) {
        ui->commet->appendPlainText("Not find localhost.key - Server can not start!");
        throw StartupException("Cannot find private key file");
    }
    if (!QFile("localhost.cert").exists()) {
        ui->commet->appendPlainText("Not find localhost.cert - Server can not start!");
        throw StartupException("Cannot find local certificate file");
    }

    if(!SSLconfiguration(sslconfig))
        throw StartupException("Impossible to set encryption configuration");

    m_pWebSocketServer->setSslConfiguration(sslconfig);

    // Check existence of (or create) the Users folder
    if (!QDir("Users").exists())
    {
        ui->commet->appendPlainText("Creating the server Users folder");
        if (!QDir().mkdir("Users")) {
            ui->commet->appendPlainText("Cannot create folder '.\\Users'");
            throw StartupException("Cannot create folder '.\\Users'");

        }
    }

    ServerDatabase database;
    try {
        database.open(defaultnamedb,QString("default"),ui.get());
    }catch ( DatabaseConnectionException& dce ) {
        ui->commet->appendPlainText("open DB connection exception in ServerLoginRequest()");
        return;

    }catch (DatabaseException& re ) {
        ui->commet->appendPlainText("open DB connection exception in ServerLoginRequest()");
        return;
    }

    try{
        int numUser = database.getMaxUserID();
        ui->commet->appendPlainText("Users in database: "+ QString::number(numUser) );
        ui->commet->appendPlainText("(INIT COMPLETE )" );

    }catch (DatabaseReadException& re) {
        ui->commet->appendPlainText("getMaxUserID query READ EXCEPTION in prepareToStart() ");
        return;
    }
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
        users.clear();
        lockwork.clear();
        workspaces.clear();

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

   // qDebug() << pSocket;

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

    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient "+client->getUsername()+" disconnected");
    ui->num_client->display(ui->num_client->intValue()-1);

    if(users.contains(client->getUsername()))
        users.remove(client->getUsername());
    clients.remove(socket);					/* remove this client from the map */

    socket->close();						/* close and destroy the socket */
    socket->deleteLater();
}

void MainWindow::onSslErrors(const QList<QSslError> & sslerror)
{
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSsl errors occurred");
    for(auto& e : sslerror){
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tError"+e.errorString());
    }
}

void MainWindow::socketAbort(QWebSocket* socket)
{
  //  qDebug()<<socket<<" invocazione metodo socket abort   "<<QThread::currentThread();

    QSharedPointer<Client> client = clients[socket];
    QString ip;
    if(socket->isValid()){
        ip=socket->peerAddress().toString();
     //   qDebug()<<"socket abort thread "<<socket->thread()->currentThread();
        socket->blockSignals(true);
        socket->flush();
        socket->close(QWebSocketProtocol::CloseCodeBadOperation);
        socket->deleteLater();
    }
    else
        socket->deleteLater();

    if(users.contains(client->getUsername()))
        users.remove(client->getUsername());
    clients.remove(socket);					/* remove this client from the map */

    if (client->isLogged())
    {
        client->logout();
        emit printUiServer("Eject "+client->getUsername()+" ip: "+ip);
    }
    else
        emit printUiServer("Shutdown connection of no logged client: "+ip);
}

void MainWindow::printUiServer(QString messageToPrint)
{
    ui->commet->appendPlainText(messageToPrint);
}

void MainWindow::printUiServerDatabase(QString messageToPrint)
{
    ui->commetdb->appendPlainText(messageToPrint);
}

void MainWindow::serverErrorConnection(QWebSocketProtocol::CloseCode closeCode)
{
    ui->commet->appendPlainText("Errors occurred during ssetup socket\n"+QString(closeCode));
}

void MainWindow::on_startserver_2_clicked() {  ui->commet->clear(); }

void MainWindow::SimpleTextMessageTest(){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSIGNAL TEST MESSAGE FROM CLIENT RECEIVED TO SERVER");
}

void MainWindow::processBinaryMessage(QByteArray message)
{
   // qDebug() << "new binary";
    QWebSocket* socket = dynamic_cast<QWebSocket*>(sender());
    if (socket == nullptr || !socket->isValid())
        return;

    QDataStream stream (&message, QIODevice::ReadOnly);
    stream.setVersion(QDataStream::Qt_5_14);
    QJsonDocument jsonDoc;
    stream >> jsonDoc;

    // Fail if the JSON is invalid.
    if (jsonDoc.isNull() || !jsonDoc.isObject()){
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tprocessBinaryMessage() error. INVALID JSON FORMAT ");
        return ;
    }

    QJsonObject request = jsonDoc.object();
    TypeOperation typeOp = (TypeOperation)(request["type"].toInt());


    ////// prva aggiunta testing mutex separatly
    ///  NEL CASO NON FUNZIOANA COME DEVE BASTA RIMUOVERE TUTTO IL CODICE COMRPESO TRA SUETE RIGHE IN BLU
    if(request.contains("URI") || request.contains("siteid"))
    {
        QString URI;
        if(request.contains("URI")){
            URI = request["URI"].toString();
          //   qDebug() << "URI" << request["URI"].toString();
        }else if(request.contains("siteid")){
            URI = request["siteid"].toString();
           // qDebug() << "siteid" << request["siteid"].toString();
        }

        //If there isn't the workspace's URI created, is not possible to delete or insert a char (case request["siteid"])

        if(!workspaces.contains(URI) && request.contains("siteid")){
             ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tInvalid Message received. Abort operation ");
             return;
        }

        if(workspaces.contains(request["URI"].toString()))
        {
         //   qDebug()<<"WORKSPACE PRESENT:"<<workspaces[URI].get()
          //          <<"WITH QMUTEX:"<<lockwork[URI];
            QThreadPool::globalInstance()->start(new Tasks(this, request, socket, clients, users, workspaces, typeOp,ui.get(), lockwork[URI]));
            return;
        }else
        {
            if(!lockwork.contains(URI)){
                lockwork.insert(URI,new QMutex);
             //   qDebug()<<"INSERTED QMUTEX:"<<lockwork[URI];
            }
         //   qDebug()<<"LIST KEY:"<<lockwork.keys();
           // qDebug()<<"LIST VALUES:"<<lockwork.values();
            //qDebug()<<"QMUTEX:"<<lockwork[URI];
            QThreadPool::globalInstance()->start(new Tasks(this, request, socket, clients, users, workspaces, typeOp,ui.get(), lockwork[URI]));
            return;
        }
    }

    ////// end  aggiunta testing

    QThreadPool::globalInstance()->start(new Tasks(this, request, socket, clients, users, workspaces, typeOp,ui.get(), &mutex));
}
