#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonDocument>
#include "buildermessageclient.h"
#include "clientfilesystem.h"
#include <QDir>
#include <QProcess>
#include <textedit.h>
#include <QDesktopWidget>
#include <QScreen>

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

void MainWindow::StartEditorText(QString fileeditor)
{
    editor= new TextEdit(secondWindows);
    QCoreApplication::setApplicationName("CoopEditorText");
    editor->setAttribute(Qt::WA_DeleteOnClose);

    const QRect availableGeometry = editor->screen()->availableGeometry();
    editor->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    editor->move((availableGeometry.width() - editor->width()) / 2,
            (availableGeometry.height() - editor->height()) / 2);

    if (!editor->load(fileeditor))
        editor->fileNew();

    editor->show();
}


void MainWindow::MessageReceivedFromServer(const QByteArray &message)
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message, &parseError);

    // Fail if the JSON is invalid.
    // Make sure the root is an object.
    int pos=0;
    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()){
        // can be bytes of file
       qDebug()<<"Possbile file in byte in arrivo";
       pos=message.indexOf("{",0);
       QByteArray dimjson(message.data(),pos);
       QByteArray header(message.data()+pos,dimjson.toInt());
       pos=pos+dimjson.toInt();
       // in header hai il json , in message il contenuto del file.
       //check if the json is correct
       QJsonParseError parseError2;
       QJsonDocument jsonDoc2 = QJsonDocument::fromJson(header, &parseError2);
       if (parseError2.error != QJsonParseError::NoError || !jsonDoc2.isObject())
            return ;
       jsonDoc=jsonDoc2;

    }

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
            StartNewWindows();
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
        case 11:{    // file in arrivo
            QString fileinarrivo(root.value("header").toString());
            qDebug()<<"file in arrivo"<<fileinarrivo;
            QFile f(QDir().tempPath()+"/"+fileinarrivo);

            if (!f.open(QIODevice::WriteOnly))
                    return;

            f.write(message.data()+pos);
            f.close();
            //QProcess::execute("gedit "+QDir().tempPath()+"/"+fileinarrivo);
            // Aperura editor per l'editing del file
            StartEditorText(QDir().tempPath()+"/"+fileinarrivo);
        }break;


        default:         return;
    }

}



