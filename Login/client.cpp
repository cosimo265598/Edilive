#include "client.h"


Client::Client(QObject *parent) :
    QObject(parent),
    stackedDialog(new StartupStackedDialog()),
    urlForConnection("wss://localhost:1234"),
    reconnectionRetries(3),
    user(nullptr),
    waitingTimer(new QTimer(this)),
    clientStatus(Startup)
{
    this->waitingTimer->setInterval(20000);

    m_webSocket = QSharedPointer<QWebSocket>( new QWebSocket("client",QWebSocketProtocol::VersionLatest,this) );

    connect(stackedDialog, &StartupStackedDialog::destroyed, [this](){this->stackedDialog = nullptr; delete this->user; this->user = nullptr;});
    connect(stackedDialog, &StartupStackedDialog::loginRequest, this, &Client::onLoginRequest);
    connect(this, &Client::loginFailure, stackedDialog, &StartupStackedDialog::onLoginFailure);
    connect(this, &Client::registrationFailure, stackedDialog, &StartupStackedDialog::onRegistrationFailure);
    connect(stackedDialog, &StartupStackedDialog::registrationRequest, this, &Client::onRegistrationRequest);

    connect(m_webSocket.get(), QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),this, &Client::onSslErrors);
    connect(m_webSocket.get(), &QWebSocket::binaryMessageReceived, this, &Client::MessageReceivedFromServer);
    connect(m_webSocket.get(), &QWebSocket::disconnected, this, &Client::onDisconnection);
    connect(m_webSocket.get(), &QWebSocket::connected, this, &Client::onConnectionSuccess);
    connect(this->waitingTimer, &QTimer::timeout, this, &Client::onConnectionFailure);
    connect(m_webSocket.get(), &QWebSocket::connected, this, &Client::onConnected);

    stackedDialog->show();
}

Client::~Client()
{
}

void Client::onLoginRequest(QString username, QString password){

    user = new User(username, password);
    this->clientStatus = LoginRequest;
    waitingTimer->start();

    waitingDialog.setText("Connection to the server...");
    waitingDialog.exec();
    m_webSocket.get()->open(this->urlForConnection);
}

void Client::onRegistrationRequest(QString username, QString password){
     user = new User(username, password);
     this->clientStatus = RegistrationRequest;

     waitingDialog.setText("Connection to the server...");
     waitingDialog.exec();
     waitingTimer->start();
     m_webSocket.get()->open(this->urlForConnection);
}

void Client::onConnected(){
    QByteArray out;

    switch(this->clientStatus){
        case LoginRequest: {     
            BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageLogin(this->user->getUsername()));
            m_webSocket.get()->sendBinaryMessage(out);
            break;
        }
        case RegistrationRequest: {
            BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageRegisterAccount(this->user->getUsername(),this->user->getPassword()));
            m_webSocket.get()->sendBinaryMessage(out);
            break;
        }
        case Disconnected: {
            waitingDialog.setText("Reconnection...");
            this->waitingTimer->stop();
            this->waitingTimer->start();
            waitingDialog.exec();
            break;
        }
    }
}


void Client::onDisconnection(){
    qDebug() << "DISCONNECTED";
    switch(this->clientStatus){
        case LoginRequest: {
            waitingDialog.close();
            emit loginFailure("Login failure, impossible to contact the server (DISCONNECTED)");
            break;
        }
        case RegistrationRequest: {
            waitingDialog.close();
            emit loginFailure("Registration failure, impossible to contact the server (DISCONNECTED)");
            break;
        }
        case Connected: {
            waitingDialog.setText("Reconnection...");
            this->waitingTimer->stop();
            this->waitingTimer->start();
            waitingDialog.exec();
            break;
        }
    }
}

void Client::onSslErrors(const QList<QSslError> &errors)
{

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    m_webSocket.get()->ignoreSslErrors();
}

void Client::StartNewWindows()
{
    /*
    secondWindows = new ClientFilesystem(this,m_webSocket.get());
    MainWindow::hide();
    secondWindows->setWindowTitle(tr("Esplora File"));
    secondWindows->setMinimumSize(QSize(800, 400));
    secondWindows->setAttribute(Qt::WA_DeleteOnClose);
    //secondWindows->setWindowFlags(Qt::Popup);
    secondWindows->show();
    */
}

void Client::StartEditorText(QString fileeditor)
{
    /*
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
    */
}


void Client::MessageReceivedFromServer(const QByteArray &message)
{
    QDataStream stream(message);
    stream.setVersion(QDataStream::Qt_5_14);
    QJsonDocument jsonDoc;

    stream >> jsonDoc;

    qDebug() << jsonDoc;

    if (jsonDoc.isNull()) {
        std::cout << "Failed to create JSON doc." << std::endl;
        return;
    }

    if (!jsonDoc.isObject()) {
        std::cout << "JSON is not an object." << std::endl;
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();

    switch (jsonObj["type"].toInt()) {
        case 2:{   // message challange login
            QString salt =jsonObj["salt"].toString();
            QString nonce =jsonObj["nonce"].toString();

            QByteArray out;
            BuilderMessageClient::MessageSendToServer(
                        out,
                        BuilderMessageClient::MessageLoginUnlock(salt,nonce,this->user->getPassword()));
            m_webSocket.get()->sendBinaryMessage(out);
        } break;

        case 4:{    // message unlock login
                qDebug() << "Successfull login";
                this->stackedDialog->close();
                this->clientStatus = Connected;
                this->homePage = new HomePage(m_webSocket.get());
                break;
        }
        case 5:{    // message  login error
                delete this->user;
                this->user = nullptr;
                emit loginFailure(jsonObj["error"].toString());
                break;
        }
        case 8:{    // message account confimed
                qDebug() << "Account created";
                this->stackedDialog->close();
                this->clientStatus = Connected;
                this->homePage = new HomePage(m_webSocket.get());
                break;
        }
        case 9:{    // message account create error
            qDebug() << "Account creation failure";
            emit registrationFailure(jsonObj["error"].toString());
            break;
        }
        case 10:{    // show dir/document for client
            this->homePage->createHomepage(jsonObj["files"].toArray());
        }break;
        case 13:{    // file gia presente
            //QMessageBox::critical(secondWindows, tr("Errore nella creazione del file"),jsonObj["error"].toString(),QMessageBox::Ok);
            qDebug() << "Errore nella creazione del file";
    }break;
        case 11:{    // file in arrivo
            QString fileName = jsonObj["fileName"].toString();
            qDebug()<<"file in arrivo"<< fileName;
            QFile file(QDir().tempPath()+"/"+ fileName);

            if (file.open(QIODevice::WriteOnly)){
                QByteArray serializedFile;
                stream >> serializedFile;
                file.write(serializedFile);
                file.close();
                //QProcess::execute("gedit "+QDir().tempPath()+"/"+fileinarrivo);
                // Aperura editor per l'editing del file
                StartEditorText(QDir().tempPath()+"/"+ fileName);
            }else
                return;
        }break;
        case 17:{
            qDebug()<<jsonObj;
            //this->homePage->getProfilePage()->viewDataOfClient(jsonObj);
        }break;

        default:         return;
    }
}


void Client::onConnectionSuccess(){
    //this->reconnectionTimer->stop();
    this->reconnectionRetries = 3;
}

void Client::onConnectionFailure(){

    qDebug() << "Failure";
    this->m_webSocket->abort();
    if(this->reconnectionRetries > 0){
        this->reconnectionRetries --;
        this->m_webSocket.get()->open(this->urlForConnection);
    }else{
        //this->secondWindows->hide();
        //MainWindow::show();
    }

}
