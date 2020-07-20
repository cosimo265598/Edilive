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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow),
    m_pWebSocketServer(nullptr)
{
    ui->setupUi(this);
   // ancora un commento come prova
    ui->stopserver->setEnabled(false);
    ui->commet->setReadOnly(true);
    m_pWebSocketServer =  QSharedPointer<QWebSocketServer>(new QWebSocketServer("SSL_Server",QWebSocketServer::SecureMode,this));

    SSLconfiguration(sslconfig);
    m_pWebSocketServer->setSslConfiguration(sslconfig);
    ui->commetdb->setReadOnly(true);

    database.open(defaultnamedb,ui.get());

}

MainWindow::~MainWindow()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
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

        return;
    }
}

void MainWindow::on_stopserver_clicked()
{
    if(m_pWebSocketServer->isListening()){
        m_pWebSocketServer->close();
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tServer stopped...");

        qDeleteAll(m_clients.begin(), m_clients.end());
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tAll client will be disconnected...");

        ui->stopserver->setEnabled(false);
        ui->startserver->setEnabled(true);
    }
}

void MainWindow::onNewConnection(){

    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    if(pSocket==nullptr || !pSocket->isValid())
        return;
    
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient connected: "+pSocket->peerName()+"IP: "+pSocket->peerAddress().toString());

    connect(pSocket, &QWebSocket::textMessageReceived, this, &MainWindow::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived,this, &MainWindow::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &MainWindow::socketDisconnected);

    m_clients.push_back(pSocket);
    ui->num_client->display(ui->num_client->intValue()+1);
 }

void MainWindow::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        //pClient->sendTextMessage("messagio ricevuto.");
        for (int i=0; i< m_clients.size();i++){
            if(m_clients.at(i)!=pClient)
                m_clients.at(i)->sendTextMessage(message);
        }
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient ha inviato:\t"+message);
    }
}

void MainWindow::processBinaryMessage(QByteArray message)
{
    // processare il messoggio , leggendo i primi 16 bit per identificare il tipo di messaggio sul socket
    // dopo chimare la funzione processmessage
    // che chiamera una funzione dentro questa classe per la sua elaborazione
    /*
    QWebSocket *pClient = dynamic_cast<QWebSocket*>(sender());
    if (pClient)
    {
        for (int i=0; i< m_clients.size();i++){
            if(m_clients.at(i)!=pClient)
                m_clients.at(i)->sendBinaryMessage(message);
        }

        User a;
        QDataStream out(&message, QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_14);
        out>>a;
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient ha inviato byte:\t"+a.print());


         Esempio di operazione
        ProcessOperation p(nullptr); -- messo nel costruttore
        quint16 tipo;
        QDataStream out(&message,QIODevice::ReadOnly);
        out.setVersion(QDataStream::Qt_5_14);
        out>>tipo;
        p.process((TypeOperation)tipo,nullptr);

    }*/

    QWebSocket* socket = dynamic_cast<QWebSocket*>(sender());
    if (socket == nullptr || !socket->isValid())
        return;
    QDataStream streamIn(&message, QIODevice::ReadOnly);	/* connect stream with socket */
    streamIn.setVersion( QDataStream::Qt_5_14);
    quint16 mType;
    QString testo;
    streamIn>>mType;
    ui->commet->appendPlainText("Tipo Messaggio: "+QString::number(mType));
    streamIn>>testo;
    ui->commet->appendPlainText("Contenuto Messaggio: "+testo);

    try {
        ProcessOperation po;

        if (mType == LoginRequest || mType == LoginUnlock || mType == AccountCreate || mType == AccountUpdate ||
            mType == Logout || mType == Simplemessage )
        {
            ui->commet->appendPlainText("Type correct");
            po.process((TypeOperation)mType, socket);
        }
        else
            ui->commet->appendPlainText("(MESSAGE ERROR) Received unexpected message: ");
    }
    catch (std::exception& me)
    {
        ui->commet->appendPlainText( me.what());
    }
}

void MainWindow::socketDisconnected()
{
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tClient disconnected");
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
    ui->num_client->display(ui->num_client->intValue()-1);

}

void MainWindow::onSslErrors(const QList<QSslError> & sslerror)
{
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tSsl errors occurred");
    for(auto e : sslerror){
        ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tError"+e.errorString());
    }
}

void MainWindow::on_startserver_2_clicked()
{
    ui->commet->clear();
}

void MainWindow::SimpleTextMessageTest(){
    ui->commet->appendPlainText(QDateTime::currentDateTime().toString()+"\tTEST MESSAGE");
}
