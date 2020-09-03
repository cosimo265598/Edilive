#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonDocument>
#include "buildermessageclient.h"
#include "clientfilesystem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fieldForSign(false);
    this->urlForConnection="wss://localhost:1234";
    m_webSocket = QSharedPointer<QWebSocket>( new QWebSocket("client",QWebSocketProtocol::VersionLatest,this) );
    connect(m_webSocket.get(), QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),this, &MainWindow::onSslErrors);
    connect(m_webSocket.get(), &QWebSocket::binaryMessageReceived, this, &MainWindow::MessageReceivedFromServer);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fieldForSign(bool state){
    if(!state){
        ui->sign->show();
        ui->login->show();

        ui->labelconfpsw->hide();
        ui->confpsw->hide();
        ui->sign_account->hide();
    }else
    {
        ui->labelconfpsw->show();
        ui->confpsw->show();
        ui->sign_account->show();

        ui->sign->hide();
        ui->login->hide();
    }
}


void MainWindow::on_sign_clicked()
{
    fieldForSign(true);
}

void MainWindow::on_back_clicked()
{
    fieldForSign(false);
}

void MainWindow::on_login_clicked()
{
    // implemntare mecccanismo di riprova collegamento
    connect(m_webSocket.get(), &QWebSocket::connected, this, &MainWindow::onConnected);
    m_webSocket.get()->open(this->urlForConnection);
}
void MainWindow::onConnected(){
    ui->status->setText("Status: Connected...");
    m_webSocket.get()->sendBinaryMessage(BuilderMessageClient::MessageLogin(ui->user->text()).toJson());
}

void MainWindow::onSslErrors(const QList<QSslError> &errors)
{

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    m_webSocket.get()->ignoreSslErrors();
}

void MainWindow::on_sign_account_clicked()
{
    // check consistenza password
    if (ui->psw->text().compare(ui->confpsw->text()) || ui->user->text().isEmpty()){
        QMessageBox::critical(this, tr("Mandatory data"),tr("Password and confirm password must be ugual.\nUsername can not be empty.\n"),QMessageBox::Ok);
        return;
    }
    connect(m_webSocket.get(), &QWebSocket::connected, this, &MainWindow::Registeruser);
    m_webSocket.get()->open(this->urlForConnection);

}
void MainWindow::Registeruser()
{
    ui->status->setText("Status: Connected...");
    m_webSocket.get()->sendBinaryMessage(BuilderMessageClient::MessageRegisterAccount(
                                      ui->user->text(),
                                      ui->psw->text(),
                                      QString(),
                                      QImage() ).toJson());

}

void MainWindow::StartNewWindows()
{
    secondWindows = new ClientFilesystem(this,m_webSocket.get());
    MainWindow::hide();
    secondWindows->setWindowTitle(tr("Esplora File"));
    secondWindows->setMinimumSize(QSize(800, 400));
    secondWindows->setAttribute(Qt::WA_DeleteOnClose);
    //secondWindows->setWindowFlags(Qt::Popup);
    secondWindows->show();
}


void MainWindow::MessageReceivedFromServer(const QByteArray &message)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message, &parseError);

    // Fail if the JSON is invalid.
    // Make sure the root is an object.

    if (parseError.error != QJsonParseError::NoError)
        return ;
    if (!jsonDoc.isObject())
        return ;

    QJsonObject root = jsonDoc.object();
    int type= root.value("type").toInt();

    switch (type) {
        case 2:{   // message challange login
            QString salt=root.value("salt").toString();
            QString nonce=root.value("nonce").toString();
            m_webSocket.get()->sendBinaryMessage(BuilderMessageClient::MessageLoginUnlock(salt,nonce,ui->psw->text()).toJson());

        } break;
        case 4:{    // message unlock login
            QMessageBox::information(this, tr("Login Status"),tr("Successfull login\n"),QMessageBox::Ok);
            // open new windows
            // start to open new windows for client file system.
            StartNewWindows();
        }break;
        case 5:{    // message  login error
            QMessageBox::critical(this, tr("Login Status"),"Loggin error: "+root.value("error").toString(),QMessageBox::Ok);

        }break;
        case 8:{    // message account confimed
            QMessageBox::information(this, tr("Account Status"),tr("Account Cretated\n"),QMessageBox::Ok);
            // open new windows

        }break;
        case 9:{    // message account create error
            QMessageBox::critical(this, tr("Account Status"),"Account create error: "+root.value("error").toString(),QMessageBox::Ok);

        }break;
        case 10:{    // show dir/document for client
            secondWindows->createHomepage(root.value("files").toArray());
        }break;
        case 13:{    // file gia presente
            QMessageBox::critical(secondWindows, tr("Errore nella creazione del file"),root.value("error").toString(),QMessageBox::Ok);
        }break;

        default:         return;
    }

}



