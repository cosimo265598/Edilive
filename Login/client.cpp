#include "client.h"
#include "connectionwaitingdialog.h"
#include <QFloat16>

Client::Client(QObject *parent) :
    QObject(parent),
    sf(nullptr),
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

    //connection texteditor e client

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
    connect(this, &Client::accountUpdateError, mainWindowStacked, &MainWindowStacked::accountUpdateError);
    connect(mainWindowStacked, SIGNAL(shareFile(QString, QString)), this, SLOT(onShareFile(QString, QString)));

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

void Client::standardInsert(QJsonObject symbol)
{
    //qDebug() << "symbol:  " << symbol;
    //qDebug()<<"Messaggio inserimento carattere: "+symbol["car"].toString();
    QChar c = QString(symbol["car"].toString())[0];
    QString id=symbol["id"].toString();
    //std::string posfraz=jsonObj["posfraz"].toString().toStdString();
    QString siteid=symbol["siteid"].toString();
    std::vector<int> v;

    for(auto val :symbol["posfraz"].toArray())
        v.push_back(val.toInt());

    QByteArray ricevo(QByteArray::fromBase64(symbol["format"].toString().toLatin1()));
    QDataStream out2(ricevo);
    QTextCharFormat fmt;
    out2>>fmt;

    Symbol s(c,siteid,v,id,fmt);
    sf->localInsert(s, subscriber.username); //INSERIMENTO NELLA STRUTTURA LOGICA
    //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE L'INSERIMENTO NELL'EDITOR DI SYMBOL S
}

void Client::startTextEditor(QString fileName)
{
    textEditor= new TextEdit(0,&this->subscriber,&listUserOnWorkspace);

    connect(textEditor, &TextEdit::close, this, &Client::onCloseTextEditor);

    QCoreApplication::setApplicationName("textEditor");
    textEditor->setAttribute(Qt::WA_DeleteOnClose);

    const QRect availableGeometry = textEditor->screen()->availableGeometry();
    textEditor->resize(availableGeometry.width() / 2, (availableGeometry.height() * 2) / 3);
    textEditor->move((availableGeometry.width() - textEditor->width()) / 2,
                     (availableGeometry.height() - textEditor->height()) / 2);

    connect(textEditor, &TextEdit::localInsertionSignal, this, &Client::localInsertion,Qt::QueuedConnection);
    connect(textEditor, &TextEdit::localDeletionSignal, this, &Client::onLocalDeletion,Qt::QueuedConnection);
    connect(textEditor, &TextEdit::removeClientWorkspace, this, &Client::onRemoveClientWorkspace,Qt::QueuedConnection);
    connect(this,&Client::updateListUsersConnected,textEditor,&TextEdit::onUpdateListUsersConnected,Qt::QueuedConnection);
    connect(this,&Client::fromServerInsertSignal, textEditor, &TextEdit::fromServerInsert,Qt::QueuedConnection);
    connect(this,&Client::fromServerDeleteSignal, textEditor, &TextEdit::fromServerDelete,Qt::QueuedConnection);
    connect(textEditor, &TextEdit::saveFile, this, &Client::saveFile,Qt::QueuedConnection);
    connect(textEditor, &TextEdit::changeCursorPositionSignal, this, &Client::changeCursorPosition,Qt::QueuedConnection);
    connect(this, &Client::fromServerChangeCursorSignal, textEditor, &TextEdit::fromServerNewCursorPosition, Qt::QueuedConnection);
    textEditor->show();


    for(Symbol s: sf->getSymbols())
            qDebug()<<s.getCar()<<"\n";

        qDebug() << "testo: " << sf->to_string();

        if (!textEditor->load(fileName, sf))
            textEditor->fileNew();
}


void Client::MessageReceivedFromServer(const QByteArray &message)
{
    QDataStream stream(message);
    stream.setVersion(QDataStream::Qt_5_13);
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

    qDebug() << jsonObj;

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
        emit receivedFileHandlers(jsonObj["files"].toArray(), jsonObj["onReload"].toString());
        break;
    }
    case 13:{    // file gia presente
        qDebug() << "Errore nella creazione del file";
        emit newFileCreationFailure(jsonObj["error"].toString());
        break;
    }
    case 11:{    // file in arrivo
        QString fileName = jsonObj["fileName"].toString();
        QString creatore = jsonObj["creator"].toString();

        sf = new SharedFile(fileName, creatore);
        qDebug()<<"file in arrivo"<< fileName;

        int last = 0;

        while (!last){
            QJsonDocument jsonDoc;
            stream >> jsonDoc;
            QJsonObject obj = jsonDoc.object();

            last = obj["last"].toInt();

            qDebug() << "Cosa mi arriva: " << obj["symbols"].toArray();
            qDebug() << "ora inserisco";
            for(auto symbol: obj["symbols"].toArray())
                this->standardInsert(symbol.toObject());
            qDebug() << "Ho inserito tutto";
        }

        qDebug() << "Apro l'editor";
        qDebug() << " connected client array "<< jsonObj["connected"].toArray();

        for(auto i : jsonObj["connected"].toArray()){
            subscriber_t s;
            s.username= i.toObject().value("username").toString();
            s.nickname = i.toObject().value("nickname").toString();
            s.serializedImage= i.toObject().value("icon").toString().toLatin1().toBase64();
            listUserOnWorkspace.append(s);
        }
        // Aperura editor per l'editing del file
        this->startTextEditor(QDir().tempPath()+"/"+ fileName);

        break;
    }
    case 17:{
        qDebug()<<"load Info    " << jsonObj;
        QByteArray serializedImage = nullptr;
        if(jsonObj["present_icon"].toBool()){
            QString stringifiedImage = jsonObj["icon"].toString();
            serializedImage = QByteArray::fromBase64(stringifiedImage.toLatin1());
        }

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
    case 21:{ // Update account error

        qDebug() << "Update Account error";
        resetUpdateUser();
        emit accountUpdateError(jsonObj["error"].toString());
    }


    case 50:{//inserimento standard
        qDebug()<<"INSERIMENTO DA SERVER";
        //
        //AREA DEBUG
        qDebug() << "INSERIMENTO CARATTERE DA SERVER";
        qDebug() << "FIle pre inserimento: " << sf->to_string();
        for(Symbol s : sf->getSymbols()){
            qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
        }
        //fine AREA DEBUG

        //
        QChar c = QString(jsonObj["car"].toString()).front();
        qDebug() << "char: " << c;

        QString id=jsonObj["id"].toString();
        QJsonArray posfraz=jsonObj["posfraz"].toArray();
        QString siteid=jsonObj["siteid"].toString();
        std::vector<int> v;

        for(QJsonValue val: posfraz)
            v.push_back(val.toInt());

        QByteArray ricevo(QByteArray::fromBase64(jsonObj["format"].toString().toLatin1()));
        QDataStream out2(ricevo);
        QTextCharFormat fmt=QTextCharFormat();
        out2>>fmt;

        Symbol s(c,siteid,v,id,fmt);
        sf->localInsert(s, subscriber.username); //INSERIMENTO NELLA STRUTTURA LOGICA
        int cn=0;
        for(Symbol k : sf->getSymbols()){
            if(k.getId()==s.getId())
                break;
            cn++;
        }
        //AREA DEBUG
        qDebug()<<"Sto inserendo carattere c="<<c<<" in pos="<<cn;
        qDebug() << "INSERIMENTO SERVER DOPO";
        qDebug() << "FIle post inserimento: " << sf->to_string();
        for(Symbol s2 : sf->getSymbols()){
            qDebug() << s2.getCar() << " con posfraz=" <<s2.getPosFraz() ;
        }
        //fine AREA DEBUG
        //
        int pos=0;
        int i_pos=0;
        for(auto d: sf->getSymbols()){
            if(d.getId()==s.getId()){
                pos=i_pos;
                break;
            }
            i_pos++;
        }
        qDebug()<< " ANDARA IN POS "<<pos;
        //INSERIMENTO NELLA
        //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE L'INSERIMENTO NELL'EDITOR DI SYMBOL S

        // recuparer a chi appartiene il carttere
        QString st(jsonObj["id"].toString());
        QString u=st.split(jsonObj["siteid"].toString()).at(1).split("_").at(1);
        qDebug()<<"QUESTO CARATTERE E' DI "<< u;

        emit fromServerInsertSignal(c,pos,u,fmt);
        //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE L'INSERIMENTO NELL'EDITOR DI SYMBOL S
        break;
    }
    case 51:{ //inserimento con conflitto
        QString stringa("Messaggio inserimento con conflitto carattere: "+jsonObj["car"].toString());
        qDebug()<<stringa;
        QChar c = QString(jsonObj["car"].toString()).front();
        QString id=jsonObj["id"].toString();
        QJsonArray nposfraz=jsonObj["newposfraz"].toArray();
        QJsonArray oposfraz=jsonObj["oldposfraz"].toArray();
        QString siteid=jsonObj["siteid"].toString();
        std::vector<int> vn, vo;
        int i =0;

        for(QJsonValue val: nposfraz)            vn.push_back(val.toInt());

        for(QJsonValue val: oposfraz)            vo.push_back(val.toInt());

        QByteArray ricevo(QByteArray::fromBase64(jsonObj["format"].toString().toLatin1()));
        QDataStream out2(ricevo);
        QTextCharFormat fmt;
        out2>>fmt;


        Symbol sn(c,siteid,vn,id,fmt);
        Symbol so(c,siteid,vo,id,fmt);
        sf->localErase(so);
        sf->localInsert(sn, subscriber.username);
        //emit fromServerDeleteSignal(so.get,this->subscriber.username);
        //emit fromServerInsertSignal(sn,pos,u,fmt);

        //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE LA CANCELLAZIONE DALL'EDITOR DI SYMBOL SO
        //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE L'INSERIMENTO NELL'EDITOR DI SYMBOL SN
        break;
    }
    case 52:{ //cancellazione
        QString stringa("Messaggio cancellazione carattere: "+jsonObj["car"].toString());
        qDebug()<<stringa;
        QChar c             =QString(jsonObj["car"].toString()).front();
        QString id          =jsonObj["id"].toString();
        QJsonArray posfraz  =jsonObj["posfraz"].toArray();
        QString siteid      =jsonObj["siteid"].toString();
        std::vector<int> v;


        for(QJsonValue val: posfraz)            v.push_back(val.toInt());

        Symbol s(c,siteid,v,id,QTextCharFormat());   // non serve sapere che formatazione ha il symbol per eliminare

        //AREA DEBUG
        qDebug() << "CANCELLAZIONE DA SERVER";
        qDebug() << "File prima: " <<sf->to_string();
        for(Symbol s1 : sf->getSymbols()){
            qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz() <<" e id="<<s1.getId();
        }
        //fine AREA DEBUG
        int counteri=0;
        for(Symbol j:sf->getSymbols()){
            if(j.getId()==s.getId()){
                break;
            }
            counteri++;
        }
        sf->localErase(s);

        //AREA DEBUG
        qDebug() << "CANCELLAZIONE DA CLIENT";
        qDebug() << "File prima: " << sf->to_string();
        for(Symbol s1 : sf->getSymbols()){
            qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz()<<" e id="<<s1.getId();
        }
        //fine AREA DEBUG
        QString st(jsonObj["id"].toString());
        QString u=st.split(jsonObj["siteid"].toString()).at(1).split("_").at(1);
        qDebug()<<st;
        qDebug()<<jsonObj["siteid"].toString();
        qDebug()<<st.split(jsonObj["siteid"].toString()).at(1);
        qDebug()<<st.split(jsonObj["siteid"].toString()).at(1).split("_");
        qDebug()<<"QUESTO CARATTERE E' DI "<<u<<" "<<jsonObj["id"].toString()<<" "<<jsonObj["siteid"].toString();
        if(st.split(jsonObj["siteid"].toString()).at(1).split("_").size()>2)
            emit fromServerDeleteSignal(counteri,u);
        else
            emit fromServerDeleteSignal(counteri,this->subscriber.username);
        //EMETTI SEGNALE CHE VA INTERCETTATO DA TEXTEDIT PER SCATENARE LA CANCELLAZIONE DALL'EDITOR DI SYMBOL S
        break;
    }
    case 53:{
        emit fromServerChangeCursorSignal(jsonObj["pos"].toInt(), jsonObj["user"].toString(), jsonObj["site"].toString());
        break;
    }
    case 100:{
        qDebug()<< "CASE 100 - update inset workspace";
        subscriber_t s;
        s.username = jsonObj["username"].toString();
        s.nickname = jsonObj["nickname"].toString();
        s.serializedImage = jsonObj["icon"].toString().toLatin1().toBase64();
        listUserOnWorkspace.append(s);
        emit updateListUsersConnected(listUserOnWorkspace.size(),s.username,QImage::fromData(s.serializedImage));
        break;
    }

    case 101:{
        qDebug()<< "CASE 101 - update inset workspace";
        emit removeConnectedUser(jsonObj["username"].toString());
        break;
    }
    case 102:{
        qDebug() << "CASE 102 - new shareFile Request";

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

void Client::onFileHandlerDbClicked(QString URI, QString fileName){
    //sf = new SharedFile(fileName.toStdString(), "notMe");
    QString stringa("Ho aggiornato il file attuale a "+URI);
    qDebug()<<stringa;
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenFile(URI, fileName));
    m_webSocket.get()->sendBinaryMessage(out);
}


void Client::onCreateNewFileRequest(QString fileName){
    sf = new SharedFile(fileName, subscriber.username);
    QString stringa("Ho aggiornato il file attuale a "+fileName);
    qDebug()<<stringa;
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageCreateNewFile(fileName));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::onDeleteFileRequest(QString URI, QString fileName){
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessagedDeleteFile(URI, fileName));
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

void Client::onCloseTextEditor()
{
    QByteArray out;
        BuilderMessageClient::MessageSendToServer(
                    out,
                    BuilderMessageClient::MessagedCloseEditor(sf->getSite()));

    //Penso vada meglio un sf puntatore, in modo da cancellarlo in questo momento.

    qDebug() << "Segnalo che non sono più nell'editor";

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
    qDebug()<<m_webSocket.get()->closeCode();
    qDebug()<<"ping called - "<< old_clientstatus << " new "<< clientStatus;
    if(m_webSocket.get()->closeCode()==QWebSocketProtocol::CloseCodeBadOperation){
        waitingDialog.stopTimerForced();
        waitingDialog.reject();
        waitingDialog.close();
        return;
    }
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

void Client::localInsertion(QChar c, int pos ,QTextCharFormat format){ //INSERIMENTO DA EDITOR
    //aggiunta  durante il salvataggio fmt , agggiunta fmt al segnale
    // in fase di cancellazione non ce bisogno dell fmt , deve essere eliminato
    // non serve sapere che tipo di formatazzione ha

    //AREA DEBUG
    /*qDebug() << "INSERIMENTO LOCALE";
    qDebug() << "FIle pre inserimento: " <<sf->to_string();
    for(Symbol s : sf->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
    }*/
    //fine AREA DEBUG*/
    qDebug()<<"BEFORE ERROR";

    if(pos==sf->getSymbols().size())
        sf->localInsert(pos-1, -2, c, QString(subscriber.username),QString("NOID"),format);
    else
        sf->localInsert(pos-1, pos, c, QString(subscriber.username),QString("NOID"),format);
    QByteArray out;
    Symbol s = sf->getSymbols()[pos];
    //AREA DEBUG
    /*qDebug() << "File post inserimento: " << sf->to_string();
    for(Symbol s1 : sf->getSymbols()){
        qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz() ;
    }*/
    //FINE AREA DEBUG*/
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageInsert(s.getCar(),s.getPosFraz(),s.getId(),s.getSite(),s.getFmt()));
    //DEBUG
    qDebug()<<"E pos dell'editor: "<<pos;
    this->m_webSocket->sendBinaryMessage(out);
}


void Client::onLocalDeletion(int pos)
{

    if(sf->getSymbols().size()==0)
        return;

    //AREA DEBUG
    /*qDebug() << "CANCELLAZIONE LOCALE";
    qDebug() << "File prima: " <<sf->to_string();
    for(Symbol s : sf->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz()<<" e id="<<s.getId();
    }*/
    //fine AREA DEBUG

    Symbol s = sf->localErase(pos,QChar());

    //AREA DEBUG
    /*qDebug()<<"Sto cancellando carattere="<<s.getCar()<<" in pos="<<pos<<" e id="<<s.getId();
    qDebug()<<"File dopo:" <<sf->to_string();
    for(Symbol s : sf->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz()<<" e id="<<s.getId();
    }*/
    //fine AREA DEBUG

    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageDelete(nullptr,s.getCar(),s.getPosFraz(),s.getId(),s.getSite()));
    this->m_webSocket->sendBinaryMessage(out);
}


void Client::onRemoveClientWorkspace(QString URI)
{
    qDebug() << "Mi rimuovo";
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageRemoveClientWorkspace(URI));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::onShareFile(QString username, QString URI)
{
    qDebug() << "Client Share";
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageShareFile(username, URI));
    this->m_webSocket->sendBinaryMessage(out);
}

void Client::saveFile(QString filename)
{
    qDebug()<<"save file "<<filename;
    QSaveFile file("/tmp/colore_prova");
    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream docFileStream(&file);

        QTextCharFormat ft;
        ft.setFontItalic(true);
        ft.setFontUnderline(true);

        qDebug()<<"File opened";
        for(Symbol s: sf->getSymbols())
            docFileStream<<s.getCar()<<ft;
       qDebug()<<"File writed";
       if (docFileStream.status() == QDataStream::Status::WriteFailed)
        {
            file.cancelWriting();
            file.commit();
        }
        file.commit();
    }
}

void Client::changeCursorPosition(int pos, QString user){
    //DA IMPLEMENTARE
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageCursorChange(pos,user, sf->getSite()));
    this->m_webSocket->sendBinaryMessage(out);
}
