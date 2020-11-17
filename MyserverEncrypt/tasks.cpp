#include "tasks.h"

Tasks::Tasks(QObject *parent,
             QJsonObject request,
             QWebSocket* socket,
             QMap<QWebSocket*, QSharedPointer<Client>>& clients,
             QMap<QString, UserData>& users,
             QMap<QString, QSharedPointer<Workspace>>& workspaces,
             TypeOperation typeOp,
             Ui::MainWindow *ui,
             QMutex *mutex) :
    QObject(parent),
    clients(clients),
    users(users),
    workspaces(workspaces),
    typeOp(typeOp),
    socket(socket),
    request(request),
    threadId(-1),
    connName(""),
    ui(ui),
    locker(mutex)
{
    connect(this,&Tasks::socketAbort, dynamic_cast<MainWindow*>(parent), &MainWindow::socketAbort, Qt::QueuedConnection);
    connect(this,&Tasks::printUiServer,dynamic_cast<MainWindow*>(parent), &MainWindow::printUiServer,Qt::QueuedConnection);
}

void Tasks::serverLoginRequest()
{
    ServerDatabase database;

    QString username = request["username"].toString();
    this->threadId = QString::number((quintptr)QThread::currentThreadId());
    QSharedPointer<Client> client = clients[socket];

    if (users.contains(username)){
        emit printUiServer("Client already logged in as '" + client->getUsername() + "'");
        this->loginError("Client: "+client->getUsername()+" is already logged in.");
        emit socketAbort(socket);

        return;
    }

    qDebug()<<username<<" Server login request  -  "<<this->thread()->currentThread()<< threadId;
    //QThread::currentThread()->msleep(10000);

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        database.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection exception in ServerLoginRequest()");
        this->loginError("Server error during login");
        emit socketAbort(socket);

        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection exception in ServerLoginRequest()");
        this->loginError("Server error during login");
        emit socketAbort(socket);

        return;
    }

    try {

        UserData user(database.readUser(username));

        //Saving all the user info in memory. Deleted if login procedure will be abort or will fail
        users.insert(user.getUsername(), user);    
        client->setUser(&users[username]);
        
    }catch (DatabaseReadException& re) {
        emit printUiServer("readUser query READ EXCEPTION in ServerLoginRequest() ");
        this->loginError("Server error during login");
        emit socketAbort(socket);

        return;
    }catch (DatabaseNotFoundException& re) {
        emit printUiServer("readUser query NOTFOUND EXCEPTION in ServerLoginRequest()");
        this->loginError("Username " + username + " not registered");
        emit socketAbort(socket);

        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("readUser query EXCEPTION in ServerLoginRequest()");
        this->loginError("Server error during login");
        emit socketAbort(socket);

        return;
    }

    //////
    qDebug()<<"Funzione onMessageChallange viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                  data,BuilderMessage::MessageChallege(users[username].getSalt(), client->challenge()));
    socket->sendBinaryMessage(data);

}

void Tasks::serverLoginUnlock()
{
    this->threadId = QString::number((quintptr)QThread::currentThreadId());
    QString token = request.value("challange").toString();
    QSharedPointer<Client> client = clients[socket];

    QByteArray data;

    if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        emit printUiServer("User " + client->getUsername() + " is logged in -  served by Thread: "+threadId);
        client->login(client->getUser());
        
        //Now that the client is loggedIn correctly, load all its file's index in memory
        QList<file_t> fileList;
        ServerDatabase db;
        try {
            connName = QString::number(QRandomGenerator::global()->generate());
            db.open(defaultnamedb,connName,ui);
        }catch (DatabaseConnectionException& re ) {
            emit printUiServer("open DB connection EXCEPTION in ServerOpenDirClient()");

            return;
        }catch (DatabaseException& re ) {
            emit printUiServer("open DB connection EXCEPTION in ServerOpenDirClient()");

            return;
        }

        try{
        fileList = db.getUserDocs(client->getUsername());

        }catch (DatabaseReadException& re ) {
            emit printUiServer("getUserDocs query READ EXCEPTION in ServerOpenDirClient()");
            return;

        }catch (DatabaseException& re ) {
            emit printUiServer("getUserDocs query EXCEPTION in ServerOpenDirClient()");
            return;
        }

        // visita della directory
        for (auto f: fileList) {
            qDebug() << "FileName: " + f.fileName;
            client->getUser()->addFile(f);
        }

        ///////

        qDebug()<<"Funzione onMessageChallangePassed viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

        QByteArray data;
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageChallegePassed(""));
        socket->sendBinaryMessage(data);

        return ;
    }else{
        users.remove(client->getUsername());
        this->loginError("Credentials inserted are not correct");
        emit socketAbort(socket);

        return;
    }
}

void Tasks::serverAccountCreate()
{
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    this->threadId = QString::number((quintptr)QThread::currentThreadId());

    QSharedPointer<Client> client = clients[socket];

    ServerDatabase database;

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        database.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error during account creation");
        emit socketAbort(socket);

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error during account creation");
        emit socketAbort(socket);

        return;
    }

    emit printUiServer("Creating new user account: "+username);

    try
    {
        UserData user(username, -1, "nickname", password, QByteArray());		/* create a new user with null profile image */

        //Try to add the new user record to the server database
        database.insertUser(user);

    }catch (DatabaseReadException& re ) {
        emit printUiServer("insertUser query READ EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error creating new account");
        emit socketAbort(socket);
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer("insertUser query WRITE EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error creating new account");
        emit socketAbort(socket);
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer("insertUser query TRANSACTION EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error creating new account");
        emit socketAbort(socket);

        return;

    }catch (DatabaseDuplicatedException& re ) {
        emit printUiServer("insertUser query DUPLICATE EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Username already existing");
        emit socketAbort(socket);

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("insertUser query EXCEPTION in ServerAccountCreate()");
        this->accountCreationError("Server error creating new account");
        emit socketAbort(socket);

        return;
    }

    ////////
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed( "Account Created Correctly"));
    socket->sendBinaryMessage(data);
}

void Tasks::serverOpenDirOfClient(QWebSocket* pushSocket)
{
    QWebSocket * socket1 = socket;

    if(pushSocket!=nullptr)
        socket1 = pushSocket;

    QSharedPointer<Client> client = clients[socket1];
    QJsonArray files;

    for (auto f: client->getUser()->getFiles()) {

        qDebug() << "FileName: " + f.fileName;

        files.append(QJsonObject{
                         {"filename", f.fileName}, // meglio file name
                         {"creator", f.creator},
                         {"created",  f.created},
                         {"URI", f.URI},
                     });
    }

    /////

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageOpenDirOfClient(files));
    socket1->sendBinaryMessage(data);

}

void Tasks:: serverUpdateProfileClient(){
    qDebug()<<"segnale di modifica dati profilo ricevuto";
    QSharedPointer<Client> client = clients[socket];

    QString nickname = request["nickname"].toString();
    QString password = request["password"].toString();
    QString stringifiedImage = request["icon"].toString();

    QByteArray serializedImage = nullptr;

    if(!stringifiedImage.isEmpty()){
        serializedImage = QByteArray::fromBase64(stringifiedImage.toLatin1());
    }

    UserData user = users[client->getUsername()];
    user.update(nickname, password, serializedImage);

    ServerDatabase db;

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        db.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverUpdateProfileClient()");
        this->accountUpdateError("Server error during account update");

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverUpdateProfileClient()");
        this->accountUpdateError("Server error during account update");

        return;
    }

    try
    {	// Add the new user record to the server database
        db.updateUser(user);
        users.remove(client->getUsername());
        user.setIcon(user.getIcon().insert(0,QByteArray("updt")));
        users.insert(user.getUsername(), user);
        client->setUser(&users[user.getUsername()]);
    }catch (DatabaseWriteException& re ) {
        emit printUiServer("updateUser query WRITE EXCEPTION in serverUpdateProfileClient()");
        this->accountUpdateError("Account Update Error");

        return;

    }catch (DatabaseException& re) {
        emit printUiServer("updateUser query EXCEPTION in serverUpdateProfileClient()");
        this->accountUpdateError("Account Update Error");
        return;
    }

    ///////////
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateSuccess("Update success"));

    socket->sendBinaryMessage(data);

}

void Tasks::serverPersonalDataOfClient()
{
    qDebug()<<"segnale di apertura dati profilo ricevuto";
    QSharedPointer<Client> client = clients[socket];

    QString username = client->getUsername();
    QString nickname = client->getUser()->getNickname();
    QByteArray serializedImage = client->getUser()->getIcon();

    /////////////

    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountInfo(username, nickname,serializedImage));

    socket->sendBinaryMessage(data);
}

void Tasks::serverCreateFileForClient()
{
    qDebug()<<"segnale nuovo file ricevuto";

    QString fileName = request["fileName"].toString();
    QSharedPointer<Client> client = clients[socket];

    //Possible to implement a better and more realistic random-URI function
    QString URI = client->getUsername()+"_"+fileName;

    QString filePath = rootPath + URI;
    QFile newFile(filePath);

    if(client->getUser()->hasFile(fileName)){
        emit printUiServer("serverCreateFileForClient() error. File with same name already exists");
        this->fileCreationError("File with same name already exists");
        return;
    }

    if (newFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QDataStream out(&newFile);
        out << " ";
        newFile.close();

    }else{
        emit printUiServer("serverCreateFileForClient() error creation file");
        this->fileCreationError("Error creation file");
        return;
    }

    ServerDatabase db;

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        db.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverCreateFileForClient()");
        this->fileCreationError("Server error creating new file");
        QFile(filePath).remove();
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverCreateFileForClient()");
        this->fileCreationError("Server error creating new file");
        QFile(filePath).remove();
        return;
    }

    try{
        file_t file;
        file.URI = URI;
        file.fileName = fileName;
        file.created = QFileInfo(newFile).created().toString();
        file.creator = client->getUsername();

        db.insertNewDoc(file);
        client->getUser()->addFile(file);

        this->serverOpenDirOfClient(nullptr);

    }catch (DatabaseWriteException& re ) {
        emit printUiServer("insertNewDoc query WRITE EXCEPTION in serverCreateFileForClient()");
        this->fileCreationError("Server error creation file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer("insertNewDoc query TRANSACTION EXCEPTION in serverCreateFileForClient()");
        this->fileCreationError("Server error deletion file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("insertNewDoc query EXCEPTION in serverCreateFileForClient()");
        this->fileCreationError("Server error creation file");
        QFile(filePath).remove();
        return;
    }
}

void Tasks::serverDeleteFileForClient()
{
    qDebug()<<"segnale delete file ricevuto";

    QString URI = request["URI"].toString();
    QString fileName = request["fileName"].toString();

    QSharedPointer<Client> client = clients[socket];

    QFile file(rootPath + URI);
    qDebug() << rootPath+URI;

    if(!client->getUser()->hasFile(fileName)){
        qDebug()<< "File not found";
        this->fileDeletionError("File Not Found");
        this->serverOpenDirOfClient(nullptr);

        return;
    }

    ServerDatabase db;

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        db.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverDeleteFileForClient()");
        this->fileDeletionError("Server error deleting file");
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverDeleteFileForClient()");
        this->fileDeletionError("Server error deleting file");
        return;
    }

    try{
        if(db.removeDocFromUser(client->getUsername(),URI)){
            file.remove();
        }

        client->getUser()->removeDocument(fileName);

        this->serverOpenDirOfClient(nullptr);

    }catch (DatabaseReadException& re ) {
        emit printUiServer("removeDocFromUser query READ EXCEPTION in in serverDeleteFileForClient()");
        this->fileDeletionError("Error deletion file");
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer("removeDocFromUser query WRITE EXCEPTION in in serverDeleteFileForClient()");
        this->fileDeletionError("Error deletion file");
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer("removeDocFromUser query TRANSACTION EXCEPTION in in serverDeleteFileForClient()");
        this->fileDeletionError("Error deletion file");
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("removeDocFromUser query EXCEPTION in in serverDeleteFileForClient()");
        this->fileDeletionError("Error deletion file");
        return;
    }
}

void Tasks::serverOpenFile()
{
    //qDebug()<<"Segnale apertura file ricevuto";
    QString URI = request["URI"].toString();
    QString fileName = request["fileName"].toString();

    QSharedPointer<Client> client = clients[socket];
    QString filePath = rootPath + URI;
    QFile filecreate(filePath);

    Workspace *w = nullptr;

    if(!client->getUser()->hasFile(fileName)){
        this->serverOpenDirOfClient(nullptr);

        return;
    }

    // OPERAZIONI SUL WORKSPACE, MI UNISCO AD UN WORKSPACE OPPURE LO CREO E MI UNISCO COME PRIMO CLIENT
    //Al posto del fileName mettere docURI

    if(!workspaces.contains(URI)){
        //QSharedPointer<Workspace> workspace(new Workspace(nullptr, fileName));
        w = new Workspace(nullptr, filePath);
        workspaces.insert(URI, QSharedPointer<Workspace>(w));
        w->addClient(socket, client);

    }
    else{
        w = workspaces[URI].get();
        w->addClient(socket, client);
    }

    qDebug() << "APERTURA FILE" <<(w->getSharedFile()->to_string());
    for(Symbol s : workspaces[URI]->getSharedFile()->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
    }

    //////////////////////
    QByteArray data;
    BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageHeaderFile(URI, client->getUsername(),  w->getClients()));
    BuilderMessage::MessageSendToClient(data, w->getSharedFile());
    socket->sendBinaryMessage(data);

   // qDebug()<< "<MAIN WINDOWS> voglio updatare tutti ";
    for(QSharedPointer<Client> cl : w->getClients()){
        if(cl->getUsername().compare(client->getUsername())!=0){
            QByteArray data;
            BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageInsertClientWorkspace(client->getUser()->getUsername(),client->getUser()->getNickname(),client->getUser()->getIcon()));
            cl->getSocket()->sendBinaryMessage(data);
        }
        qDebug()<<data;
    }
}

void Tasks::serverInsertionChar(){
   QChar c = QString(request["car"].toString()).front();
   QString id=request["id"].toString();
   QJsonArray posfraz=request["posfraz"].toArray();
   QString siteid=request["siteid"].toString();

   std::vector<int> v;

   for(QJsonValue val: posfraz)    v.push_back(val.toInt());

   QByteArray ricevo(QByteArray::fromBase64(request["format"].toString().toLatin1()));
   QDataStream out2(ricevo);
   QTextCharFormat fmt;
   out2>>fmt;

   Symbol old(c,siteid,v,id,fmt);
   QString caller =clients[socket]->getUsername();
       /*
       workspaces[QString::fromStdString(s.getSite())].get()->insertChar(s, "server", clientSocket, caller);
       */
       QSharedPointer<Workspace> w =  workspaces[(old.getSite())];

       SharedFile *sf = w->getSharedFile();
       int esito = sf->localInsert(old, "server");

       //== CONFLITTO ==

       if(esito==1){
           qDebug()<< "INSERIMENTO da CLIENT con CONFLITTO -- char="<< c << ", posf=" << (posfraz) << ", id="<<(id);

           std::vector<Symbol> simboli = sf->getSymbols();
           std::vector<int> nuovapos;
           for(Symbol ss : simboli)
               if(ss.getId()==old.getId()){
                   nuovapos=ss.getPosFraz();
                   break;
               }
           Symbol news(old.getCar(),old.getSite(), nuovapos, old.getId(),old.getFmt());
           QByteArray data1;
           BuilderMessage::MessageSendToClient(
                       data1,BuilderMessage::MessageConflictInsert(news.getCar(), news.getPosFraz(), old.getPosFraz(), news.getId(), news.getSite(), news.getFmt()));

           socket->sendBinaryMessage(data1);
           for(QSharedPointer<Client> cl : w->getClients()){
                   QByteArray data;
                   BuilderMessage::MessageSendToClient(
                               data,BuilderMessage::MessageInsert(news.getCar(), news.getPosFraz(), (news.getId()), (news.getSite()), news.getFmt()));

                   cl->getSocket()->sendBinaryMessage(data);
           }
       }
       else{
           qDebug()<< "INSERIMENTO da CLIENT -- char="<< c << ", posf=" << (posfraz) << ", id="<<(id);
           for(QSharedPointer<Client> cl : w->getClients()){
               if(cl->getUsername()!=caller){

                   QByteArray data;
                   BuilderMessage::MessageSendToClient(
                               data,BuilderMessage::MessageInsert(old.getCar(), old.getPosFraz(), old.getId(), old.getSite(),old.getFmt()/*,"no"*/));

                   cl->getSocket()->sendBinaryMessage(data);
               }
           }
       }
       //AREA DEBUG
       qDebug() << "File POST inserimento: " << sf->to_string();
       for(Symbol s : sf->getSymbols()){
           qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
       }
       //fine AREA DEBUG

}
void Tasks::serverDeletionChar(){
    QChar c             =QString(request["car"].toString()).front();
    QString id          =request.value("id").toString();
    QJsonArray posfraz  =request.value("posfraz").toArray();
    QString siteid      =request.value("siteid").toString();
    std::vector<int> v;
    qDebug()<< "CANCELLAZIONE da CLIENT -- char="<< c << ", posf=" << (posfraz) << ", id="<<(id);

    for(QJsonValue val: posfraz)    v.push_back(val.toInt());

    Symbol s(c,siteid,v,id,QTextCharFormat());

    QString caller =clients[socket]->getUsername();
    //workspaces[QString::fromStdString(s.getSite())].get()->localErase(s, "server", caller);

    QSharedPointer<Workspace> w =  workspaces[(s.getSite())];
    SharedFile *sf = w->getSharedFile();

    sf->localErase(s);
    //AREA DEBUG
        qDebug() << "File POST cancellazione: " << sf->to_string();
        for(Symbol s1 : sf->getSymbols()){
            qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz()<<" e id="<<(s1.getId());
        }
        //fine AREA DEBUG

    //INFORMA TUTTI I CLIENT TRANNE IL CHIAMANTE DELLA CANCELLAZIONE
    for(QSharedPointer<Client> cl : w->getClients()){
        if(cl->getUsername()!=caller){
            QByteArray data;
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageDelete(s.getCar(), s.getPosFraz(), s.getId(), s.getSite()));

            cl->getSocket()->sendBinaryMessage(data);
        }
    }
}


void Tasks::serverRemoveClientFromWorkspace(){
    qDebug() << "ricevuto segnale di remove client from workspace";
    QString URI = request["URI"].toString();

    QString caller = clients[socket]->getUsername();
    qDebug() << "IO SONO: " << caller;

    Workspace *w = workspaces[URI].get();

    w->removeClient(socket);
    qDebug() << "OK rimosso client,  n client: " << w->getClients().size() << "In workspace: " << URI;

    // == Every time a client is removed from the workspace, a persistante file saving is performed ==

    //Safe way to save a file. All the operation are done on a temporary unique-random named file.
    //Only if the commit() return true the original file is modified
    QSaveFile file(rootPath + URI);

    if (file.open(QIODevice::WriteOnly))
    {
        file.seek(0);
        QDataStream docFileStream(&file);

        qDebug()<<"File opened";
        for(Symbol s: w->getSharedFile()->getSymbols())
            docFileStream<<s.getCar()<<s.getFmt();

       qDebug()<<"File writed";
       if (docFileStream.status() == QDataStream::Status::WriteFailed)
        {
            file.cancelWriting();
            file.commit();
        }
        file.commit();
    }

    if(w->getClients().size()<=0){
        this->workspaces.remove(URI);
        qDebug() << "Rimosso workspace, numero workspaces: " << workspaces.size();
    }else{

        /////////////////
        w->removeClient(socket);
        for(auto cl : workspaces[URI]->getClients()){
            qDebug() << "Mi devo rimuovere dagli altri";
            if(cl->getUsername()!=caller){
                QByteArray data;
                BuilderMessage::MessageSendToClient(
                            data,BuilderMessage::MessageRemoveClientFromWorkspace(caller));
                cl->getSocket()->sendBinaryMessage(data);
            }
        }
    }
}


void Tasks::serverShareFile()
{
    qDebug() << "ricevuto segnale shareFile";

    QString URI = request["URI"].toString();
    QString userToShare = request["username"].toString();

    QSharedPointer<Client> client = clients[socket];

    ServerDatabase db;

    try {
        connName = QString::number(QRandomGenerator::global()->generate());
        db.open(defaultnamedb,connName,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverShareFile()");
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("open DB connection EXCEPTION in serverShareFile()");
        return;

    }

    try{
        if(db.addDocToUser(userToShare, URI)){
            for(auto client : clients.values()){
                if(client->getUsername() == userToShare){
                    file_t file = db.readFile(URI);
                    client->getUser()->addFile(file);
                    //emit requestShareFile(client->getSocket(), URI, client->getUsername()); // In future, push request if client connected asking if accept or decline share request
                    this->serverOpenDirOfClient(client->getSocket());

                    break;
                }
            }
        }
    }catch (DatabaseNotFoundException& re ) {
        emit printUiServer("addDocToUser query NOT FOUND EXCEPTION in serverShareFile()");
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer("addDocToUser query WRITE EXCEPTION in serverShareFile()");
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer("addDocToUser query TRANSACTION EXCEPTION in serverShareFile()");
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer("addDocToUser query EXCEPTION in serverShareFile()");
        return;
    }

}

void Tasks::changeCursorPosition(){
    int pos     =request["pos"].toInt();
    QString user=request["user"].toString();
    QString site=request["siteid"].toString();
    QSharedPointer<Workspace> w =  workspaces[site];
    for(QSharedPointer<Client> cl : w->getClients()){
        if(cl->getUsername()!=user){
            qDebug()<<"Sto informando della cancellazione client: "<<cl->getUsername();
            QByteArray data;
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageCursorChange(pos, user, site));
            cl->getSocket()->sendBinaryMessage(data);
        }
    }
}


///////////////

void Tasks::loginError(QString errorMessage)
{
    qDebug()<<"Funzione onLoginError viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,
                BuilderMessage::MessageLoginError(errorMessage));
    socket->sendBinaryMessage(data);
}

void Tasks::accountCreationError(QString error){

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountCreationError(error));
    socket->sendBinaryMessage(data);
}

void Tasks::accountUpdateError(QString error)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountUpdateError(error));
   socket->sendBinaryMessage(data);
}

void Tasks::fileDeletionError(QString error)
{
    QByteArray data;

    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageFileDeletionError(error));
    socket->sendBinaryMessage(data);
}

void Tasks::fileCreationError(QString error)
{
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageFileCreationError(error));
    socket->sendBinaryMessage(data);
}



void Tasks::run(){
    switch (typeOp) {

        case TypeOperation::LoginRequest:               this->serverLoginRequest();                 break;
        case TypeOperation::LoginUnlock:                this->serverLoginUnlock();                  break;
        case TypeOperation::AccountCreate:              this->serverAccountCreate();                break;
        case TypeOperation::OpenDirectory:              this->serverOpenDirOfClient(nullptr);       break;
        case TypeOperation::ProfileData:                this->serverPersonalDataOfClient();         break;
        case TypeOperation::AccountUpdate:              this->serverUpdateProfileClient();          break;
        case TypeOperation::CreateFile:                 this->serverCreateFileForClient();          break;
        case TypeOperation::DeleteFile:                 this->serverDeleteFileForClient();          break;
        case TypeOperation::OpenFile:                   this->serverOpenFile();                     break;
        case TypeOperation::InsertionChar:              this->serverInsertionChar();                break;
        case TypeOperation::DeletionChar:               this->serverDeletionChar();                 break;
        case TypeOperation::RemoveClientFromWorkspace:  this->serverRemoveClientFromWorkspace();    break;
        case TypeOperation::ShareFile:                  this->serverShareFile();                    break;
        case TypeOperation::changeCursorPosition:       this->changeCursorPosition();               break;

        default: qDebug() << "No Task";
    }
}

Tasks::~Tasks()
{
    //Sistema
    if(QSqlDatabase::database(connName).isValid())
        QSqlDatabase::removeDatabase(connName);
}
