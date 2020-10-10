#include "client.h"
#include "connectionwaitingdialog.h"

Client::Client(QObject *parent) :
    QObject(parent),
    stackedDialog(new StartupStackedDialog()),
    urlForConnection("wss://localhost:1234"),
    reconnectionRetries(3),
    waitingTimer(new QTimer(this)),
    clientStatus(Startup)
{
    m_webSocket = QSharedPointer<QWebSocket>( new QWebSocket("client",QWebSocketProtocol::VersionLatest,this) );
    connect(m_webSocket.get(), QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),this, &Client::onSslErrors);
    connect(m_webSocket.get(), &QWebSocket::binaryMessageReceived, this, &Client::MessageReceivedFromServer);
    connect(m_webSocket.get(), &QWebSocket::disconnected, this, &Client::onDisconnection);
    //connect(m_webSocket.get(), &QWebSocket::connected, this, &Client::onConnectionSuccess);
    //connect(this->waitingTimer, &QTimer::timeout, this, &Client::onConnectionFailure);
    connect(m_webSocket.get(), &QWebSocket::connected, this, &Client::onConnected);


    //connection waitingdialog connect signal
    connect(m_webSocket.get(), &QWebSocket::stateChanged, &waitingDialog, &ConnectionWaitingDialog::changeState);
    connect(&waitingDialog, &ConnectionWaitingDialog::tryToConnectAgain,this,&Client::ping);
    connect(m_webSocket.get(), QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &Client::errorSocket);
    connect(&waitingDialog, &ConnectionWaitingDialog::finished,this,&Client::closeControll);

    //connections StartupDialog
    connect(stackedDialog, &StartupStackedDialog::loginRequest, this, &Client::onLoginRequest);
    connect(this, &Client::loginFailure, stackedDialog, &StartupStackedDialog::onLoginFailure);
    connect(this, &Client::registrationFailure, stackedDialog, &StartupStackedDialog::onRegistrationFailure);
    connect(stackedDialog, &StartupStackedDialog::registrationRequest, this, &Client::onRegistrationRequest);

    resetUser();
    this->waitingTimer->setInterval(6000);

    stackedDialog->show();
}

Client::~Client()
{
}


void Client::onLoginRequest(QString username, QString password){

    user.username = username;
    user.password = password;

    this->clientStatus = LoginRequest;
    old_clientstatus=clientStatus;
    m_webSocket.get()->open(this->urlForConnection);
}

void Client::onRegistrationRequest(QString username, QString password){
     user.username = username;
     user.password = password;

    qDebug()<<"onRegistation request";
     old_clientstatus=clientStatus;
     this->clientStatus = RegistrationRequest;
     m_webSocket.get()->open(this->urlForConnection);
}

void Client::onConnected(){
    QByteArray out;

    switch(this->clientStatus){
        case LoginRequest: {
            old_clientstatus=clientStatus;
            BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageLogin(user.username));
            m_webSocket.get()->sendBinaryMessage(out);
            break;
        }
        case RegistrationRequest: {
            old_clientstatus=clientStatus;
            BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageRegisterAccount(user.username,user.password));
            m_webSocket.get()->sendBinaryMessage(out);
            break;
        }
        case Disconnected: {
            break;
        }
        case ReConnect:{
            // make login again
            qDebug()<<"Reconnet form login o registartion " <<old_clientstatus;
            if(old_clientstatus==LoginRequest){
                BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageLogin(user.username));
                m_webSocket.get()->sendBinaryMessage(out);
                old_clientstatus=LoginRequest;
            }
            else if(old_clientstatus==RegistrationRequest){
                BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageRegisterAccount(user.username,user.password));
                m_webSocket.get()->sendBinaryMessage(out);
                old_clientstatus=RegistrationRequest;
            }else{
                BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageLogin(user.username));
                m_webSocket.get()->sendBinaryMessage(out);
                old_clientstatus=ReConnect;
            }
        break;
        }
    }
}

void Client::resetUser(){
    user.username.clear();
    user.password.clear();
}

void Client::onDisconnection(){
    qDebug() << "DISCONNECTED";
    switch(this->clientStatus){
        case LoginRequest: {
            emit loginFailure("Login failure, impossible to contact the server (DISCONNECTED)");
            break;
        }
        case RegistrationRequest: {
            emit registrationFailure("Registration failure, impossible to contact the server (DISCONNECTED)");
            break;
        }
        case Connected: {
            break;
        }
        case ReConnect:{
            break;
        }
        default:
            return;
    }
}

void Client::onSslErrors(const QList<QSslError> &errors)
{

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.
    m_webSocket.get()->ignoreSslErrors();
}

void Client::createMainWindowStacked()
{
    this->mainWindowStacked = new MainWindowStacked();

    //connections MainWindowStacked

    connect(this, &Client::receivedFileHandlers, mainWindowStacked, &MainWindowStacked::receivedFileHandlers);
    connect(mainWindowStacked, &MainWindowStacked::fileHandlerDbClicked, this, &Client::onFileHandlerDbClicked);
    connect(this, &Client::loadSubscriberInfo, mainWindowStacked, &MainWindowStacked::loadSubscriberInfo);
    connect(mainWindowStacked, &MainWindowStacked::createNewFileRequest, this, &Client::onCreateNewFileRequest);
    connect(this, &Client::newFileCreationFailure, mainWindowStacked, &MainWindowStacked::newFileCreationFailure);
    connect(mainWindowStacked, &MainWindowStacked::deleteFileRequest, this, &Client::onDeleteFileRequest);
    connect(mainWindowStacked, &MainWindowStacked::updateProfileRequest, this, &Client::onUpdateProfileRequest);
    connect(this, &Client::updateSuccess, mainWindowStacked, &MainWindowStacked::updateSuccess);
    connect(mainWindowStacked, &MainWindowStacked::resetSubscriberInfo, [this](){emit loadSubscriberInfo(subscriber.username, subscriber.nickname, subscriber.serializedImage);});

    subscriberInfoRequest();
    fileHandlersRequest();

    this->mainWindowStacked->show();
}

void Client::subscriberInfoRequest(){

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageSubscriberInfoRequest());
     m_webSocket.get()->sendBinaryMessage(out);
}


void Client::fileHandlersRequest(){
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenDir());
     m_webSocket.get()->sendBinaryMessage(out);
}

void Client::startTextEditor(QString fileName)
{
    textEditor= new TextEdit();
    QCoreApplication::setApplicationName("textEditor");
    textEditor->setAttribute(Qt::WA_DeleteOnClose);

    const QRect availableGeometry = textEditor->screen()->availableGeometry();
    textEditor->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    textEditor->move((availableGeometry.width() - textEditor->width()) / 2,
            (availableGeometry.height() - textEditor->height()) / 2);

    if (!textEditor->load(fileName))
        textEditor->fileNew();

    textEditor->show();
}


void Client::MessageReceivedFromServer(const QByteArray &message)
{
    QDataStream stream(message);
    stream.setVersion(QDataStream::Qt_5_14);
    QByteArray out;
    QJsonDocument jsonDoc;

    stream >> jsonDoc;

    //qDebug() << jsonDoc;

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
                        BuilderMessageClient::MessageLoginUnlock(salt,nonce,user.password));
            m_webSocket.get()->sendBinaryMessage(out);
            break;
        }
        case 4:{    // message unlock login
                old_clientstatus=clientStatus;
                clientStatus=Connected;
                qDebug()<<"Successfull login : old "<<old_clientstatus<<" new "<<clientStatus;

                if(old_clientstatus==LoginRequest || old_clientstatus==RegistrationRequest){
                    qDebug()<<"QUESTO IF ";
                    this->stackedDialog->close();
                    this->stackedDialog=nullptr;
                    createMainWindowStacked();
                    return;
                }
                if(old_clientstatus==ReConnect ){
                    qDebug()<<"SECODNO IF ";

                    if(stackedDialog!=nullptr){
                        this->stackedDialog->close();
                        this->stackedDialog=nullptr;
                        createMainWindowStacked();
                    }
                }
                resetUser();
                qDebug()<<"created";
                break;
        }
        case 5:{    // message  login error
                waitingTimer->stop();
                waitingDialog.hide();

                resetUser();
                emit loginFailure(jsonObj["error"].toString());
                break;
        }
        case 8:{    // message account confimed
                qDebug() << "Account created";
                //this->clientStatus = Connected;
                // Automatic login after correct registration
                BuilderMessageClient::MessageSendToServer(out,BuilderMessageClient::MessageLogin(user.username));
                m_webSocket.get()->sendBinaryMessage(out);
                break;
        }
        case 9:{    // message account create error
            qDebug() << "Account creation failure";
            resetUser();
            emit registrationFailure(jsonObj["error"].toString());
            break;
        }
        case 10:{    // show dir/document for client
            emit receivedFileHandlers(jsonObj["files"].toArray());
            break;
        }
        case 13:{    // file gia presente
            qDebug() << "Errore nella creazione del file";
            emit newFileCreationFailure(jsonObj["error"].toString());
            break;
        }
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
                this->startTextEditor(QDir().tempPath()+"/"+ fileName);
            }else
                return;

            break;
        }
        case 17:{
            qDebug()<<"load Info";
            QByteArray serializedImage;
            if(jsonObj["icon_present"].toBool()){
                auto const encoded = jsonObj["icon"].toString().toLatin1();
                serializedImage = QByteArray::fromBase64(encoded);
            }else
                serializedImage = nullptr;

            subscriber.username = jsonObj["username"].toString();
            subscriber.nickname = jsonObj["nickname"].toString();
            subscriber.serializedImage = serializedImage;

            emit loadSubscriberInfo(subscriber.username, subscriber.nickname, subscriber.serializedImage);
            break;
        }

        case 19:{    // file not found
            qDebug() << "Errore nella eliminazione del file";
            //emit newFileCreationFailure(jsonObj["error"].toString());
            break;
        }
        case 20:{
            if(updateUser.nickname!=nullptr)
                subscriber.nickname = updateUser.nickname;
            if(updateUser.serializedImage!=nullptr)
                subscriber.serializedImage = updateUser.serializedImage;

            emit loadSubscriberInfo(subscriber.username, subscriber.nickname, subscriber.serializedImage);
            resetUpdateUser();
        break;
        }


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
}

void Client::onFileHandlerDbClicked(QString fileName){

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenFile(fileName));
    m_webSocket.get()->sendBinaryMessage(out);
}


void Client::onCreateNewFileRequest(QString fileName){

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageCreateNewFile(fileName));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::onDeleteFileRequest(QString fileName){
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessagedDeleteFile(fileName));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::onUpdateProfileRequest(updateUser_t updateUser){
    qDebug() << "update";
    this->updateUser = updateUser;
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessagedUpdateProfileRequest(updateUser.nickname, updateUser.password, updateUser.serializedImage));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::resetUpdateUser(){
    updateUser.username.clear();
    updateUser.nickname.clear();
    updateUser.password.clear();
    updateUser.serializedImage.clear();
}

void Client::ping()
{
    qDebug()<<"ping called - "<< old_clientstatus << " new "<< clientStatus;
    this->clientStatus=ReConnect;
    m_webSocket.get()->open(this->urlForConnection);
}

void Client::errorSocket(QAbstractSocket::SocketError error)
{
    qDebug()<<"Socket error: - "<<error;
}

void Client::closeControll()
{
    if(waitingDialog.result()==QDialog::DialogCode::Rejected){
        qDebug()<<"No way to recover the connectio - app should be close";
        if(mainWindowStacked!=nullptr){
             stackedDialog = new StartupStackedDialog();
             stackedDialog->show();
             mainWindowStacked->close();
        }

    }else{
        qDebug()<<"Connection recovered";
    }
}


/*
//Serve in effettivo salvare l'immagine in locale?
QByteArray Client::saveAccountImage(QString serializedImage){

     auto const encoded = serializedImage.toLatin1();
     QImage image;
     image.loadFromData(QByteArray::fromBase64(encoded), "PNG");

     // DA SISTEMARE IL PATH in modo che sia indipendente (con currentPath, ritorna il path con la cartella temporanea di debug)
     QString path = QDir().homePath()+ "/QtProjects/pds-project/myservertest/Login/images/default.png";

     if(QFile::exists(path)){
         qDebug() << "esiste";
         QFile file(path);
         file.setPermissions(file.permissions() |
                             QFileDevice::WriteOwner |
                             QFileDevice::WriteUser |
                             QFileDevice::WriteGroup |
                             QFileDevice::WriteOther);
         qDebug() << "Rimosso" << file.remove();
     }

     QFile file(path);

     if(file.open(QIODevice::ReadWrite)) {
        image.save(&file, "PNG");
     }else {
        qDebug() << "Can't open file";
     }

     file.seek(0);
     QByteArray out = file.readAll();
     file.close();
     return out;
}

*/
