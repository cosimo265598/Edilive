#include "tasks.h"

Tasks::Tasks(QObject *parent,
             QJsonObject request,
             QWebSocket* socket,
             QMap<QWebSocket*, QSharedPointer<Client>>& clients,
             QMap<QString, UserData>& users,
             QMap<QString, QSharedPointer<Workspace>>& workspaces,
             TypeOperation typeOp,
             Ui::MainWindow *ui) :
    QObject(parent),
    clients(clients),
    users(users),
    workspaces(workspaces),
    typeOp(typeOp),
    socket(socket),
    request(request),
    threadId(-1),
    ui(ui)
{
}

void Tasks::serverLoginRequest()
{
    ServerDatabase database;

    QString username = request["username"].toString();
    this->threadId = QString::number((quintptr)QThread::currentThreadId());
    QSharedPointer<Client> client = clients[socket];

    if (client.get()->isLogged()){
        emit printUiServer("Client already logged in as '" + client->getUsername() + "'");
        emit loginError(socket, "Client: "+client->getUsername()+" is already logged in.");
        emit socketAbort(socket);

        return;
    }

    qDebug()<<username<<" Server login request  -  "<<this->thread()->currentThread()<< threadId;
    //QThread::currentThread()->msleep(10000);

    try {
        database.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit loginError(socket, "Server error during login");
        emit socketAbort(socket);

        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit loginError(socket, "Server error during login");
        emit socketAbort(socket);

        return;
    }

    try {

        UserData user(database.readUser(username));

        //Saving all the user info in memory. Deleted if login procedure will be abort or will fail
        users.insert(user.getUsername(), user);

    }catch (DatabaseReadException& re) {
        emit printUiServer(re.what());
        emit loginError(socket, "Server error during login");
        emit socketAbort(socket);

        return;
    }catch (DatabaseNotFoundException& re) {
        emit printUiServer(re.what());
        emit loginError(socket, "Username " + username + " not registered");
        emit socketAbort(socket);

        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit loginError(socket, "Server error during login");
        emit socketAbort(socket);

        return;
    }

    // client not logged
    emit messageChallenge(socket, QString(users[username].getSalt()), QString(client->challenge(&users[username])));

    /*
    //////
    qDebug()<<"Funzione onMessageChallange viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

    QByteArray data;
    BuilderMessage::MessageSendToClient(
                  data,BuilderMessage::MessageChallege(users[username].getSalt(), client->challenge(&users[username])));
    socket->sendBinaryMessage(data);
    */
}

void Tasks::serverLoginUnlock()
{
    this->threadId = QString::number((quintptr)QThread::currentThreadId());
    QString token = request.value("challange").toString();
    QSharedPointer<Client> client = clients[socket];

    QByteArray data;

    /*
    if (client->isLogged()){
        emit loginError(socket, "You have to loggin before use the platform");
        return ;
    */

    if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        emit printUiServer("User " + client->getUsername() + " is logged in -  served by Thread: "+threadId);
        client->login(client->getUser());

        emit messageChallegePassed(socket, QString());

        /*
        ///////

        qDebug()<<"Funzione onMessageChallangePassed viene servuita da "<<QThread::currentThread()<<" id = "<<QString::number((quintptr)QThread::currentThreadId());

        QByteArray data;
        BuilderMessage::MessageSendToClient(
                    data,BuilderMessage::MessageChallegePassed(""));
        socket->sendBinaryMessage(data);
        */

        return ;
    }else{
        users.remove(client->getUsername());
        emit loginError(socket, "Credentials inserted are not correct");
        emit socketAbort(socket);

        return ;
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
        database.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket, "Server error during account creation");
        emit socketAbort(socket);

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket, "Server error during account creation");
        emit socketAbort(socket);

        return;
    }

    emit printUiServer("Creating new user account: "+username);

    try
    {
        UserData user(username, -1, "nickname", password, QImage(":/images/default.png"));		/* create a new user		*/
        //Try to add the new user record to the server database
        database.insertUser(user);

        users.insert(username, user);               /* insert new user in map	*/
        qDebug() << users[username].getNickname() << " " << users[username].getUserId() << " " ;

    }catch (DatabaseReadException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket, "Server error creating new account");
        emit socketAbort(socket);
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket,  "Server error creating new account");
        emit socketAbort(socket);
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket,  "Server error creating new account");
        emit socketAbort(socket);

        return;

    }catch (DatabaseDuplicatedException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket, "Username already existing");
        emit socketAbort(socket);

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit accountCreationError(socket, "Server error creating new account");
        emit socketAbort(socket);

        return;
    }

    emit accountConfirmed(socket, "Account Created Correctly");

    /*
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageAccountConfirmed( "Account Created Correctly"));
    socket->sendBinaryMessage(data);
    */
    return;

}

void Tasks::serverOpenDirOfClient(QWebSocket* pushSocket)
{
    QWebSocket * socket1 = socket;

    if(pushSocket!=nullptr)
        socket1 = pushSocket;

    QSharedPointer<Client> client = clients[socket1];
    QJsonArray files;
    QList<file_t> fileList;

    ServerDatabase db;

    try {
        db.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());

        return;
    }

    try{
    fileList = db.getUserDocs(client->getUsername());

    }catch (DatabaseReadException& re ) {
        emit printUiServer(re.what());
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        return;
    }

    // visita della directory
    for (auto f: fileList) {

        qDebug() << "FileName: " + f.fileName;

        files.append(QJsonObject{
                         {"filename", f.fileName}, // meglio file name
                         {"creator", f.creator},
                         {"created",  f.created},
                         {"URI", f.URI},
                         {"size", QFileInfo(rootPath+f.URI).size()}
                     });
    }

        //socket1 = socket;
        emit openDirOfClient(socket1, files);

    /*
    /////

    QThread::currentThread()->msleep(10000);
    QByteArray data;
    BuilderMessage::MessageSendToClient(
                data,BuilderMessage::MessageOpenDirOfClient(files));
    socket1->sendBinaryMessage(data);
    */

}

void Tasks:: serverUpdateProfileClient(){
    qDebug()<<"segnale di modifica dati profilo ricevuto";

    ServerDatabase database;

    QString nickname = request["nickname"].toString();
    QString password = request["password"].toString();
    QString stringifiedImage = request["image"].toString();

    QSharedPointer<Client> client = clients[socket];
    QByteArray data;
    QImage image{};
    if(!stringifiedImage.isEmpty())
        image.loadFromData(QByteArray::fromBase64(stringifiedImage.toLatin1()));

    UserData user = users[client->getUsername()];
    user.update(nickname, password, image);

    ServerDatabase db;

    try {
        db.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer("Databaseread problem during the query execution");
        emit accountUpdateError(socket, "Server error during account update");

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer("Databaseread problem during the query execution");
        emit accountUpdateError(socket, "Server error during account update");

        return;
    }

    try
    {	// Add the new user record to the server database
        database.updateUser(user);

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
       emit accountUpdateError(socket, "Account Update Error");

        return;

    }catch (DatabaseException& re) {
        emit printUiServer(re.what());
        emit accountUpdateError(socket, "Account Update Error");
        return;
    }

    users[client->getUsername()] = user;
    emit accountUpdateSuccess(socket, "Update success");
}

void Tasks::serverPersonalDataOfClient()
{
    qDebug()<<"segnale di apertura dati profilo ricevuto";
    QSharedPointer<Client> client = clients[socket];

    QString username = users[client->getUsername()].getUsername();
    QString nickname = users[client->getUsername()].getNickname();
    QImage image     = users[client->getUsername()].getIcon();

    emit personalDataOfClient(socket, username, nickname, image);
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

    if(newFile.exists()){
        emit printUiServer("File with same name already exists");
        emit fileCreationError(socket, "File with same name already exists");
        return;
    }

    if (newFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&newFile);
        out << "";
        newFile.close();

    }else{
        emit printUiServer("Error creation file");
        emit fileCreationError(socket, "Error creation file");
        return;
    }

    ServerDatabase db;

    try {
        db.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Server error creating new file");
        QFile(filePath).remove();
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Server error creating new file");
        QFile(filePath).remove();
        return;
    }


    try{
        db.insertNewDoc(URI,fileName,client->getUsername(), QFileInfo(newFile).created().toString());
        this->serverOpenDirOfClient(nullptr);

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Server error creation file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Server error deletion file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Server error creation file");
        QFile(filePath).remove();
        return;
    }
}

void Tasks::serverDeleteFileForClient()
{
    qDebug()<<"segnale delete file ricevuto";

    QString URI = request["URI"].toString();
    QSharedPointer<Client> client = clients[socket];

    QFile file(rootPath + URI);
    qDebug() << rootPath+URI;
    if(!file.exists()){
        qDebug()<< "File not found";
        emit fileDeletionError(socket, "File Not Found");
        return;
    }

    ServerDatabase db;

    try {
        db.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit fileDeletionError(socket, "Server error deleting file");
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileDeletionError(socket, "Server error deleting file");
        return;
    }

    try{
        if(db.removeDocFromUser(client->getUsername(),URI))
            file.remove();

        this->serverOpenDirOfClient(nullptr);

    }catch (DatabaseReadException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error deletion file");
        return;
    }
}

void Tasks::serverOpenFile()
{
    qDebug()<<"Segnale apertura file ricevuto";
    QString URI = request["URI"].toString();

    QSharedPointer<Client> client = clients[socket];
    QString filePath = rootPath + URI;
    QFile filecreate(filePath);
    if(!QFile(filePath).exists()){
       return; // Messaggio errore file not found o qualcosa del genere
    }

    // OPERAZIONI SUL WORKSPACE, MI UNISCO AD UN WORKSPACE OPPURE LO CREO E MI UNISCO COME PRIMO CLIENT
    //Al posto del fileName mettere docURI

    qDebug() << URI;

    if(!workspaces.contains(URI)){
        //QSharedPointer<Workspace> workspace(new Workspace(nullptr, fileName));
        workspaces.insert(URI, QSharedPointer<Workspace>(new Workspace(nullptr, filePath)));
        workspaces[URI].get()->addClient(socket, client);

    }
    else{
        qDebug() << "Adding current client to existing workspace";
        workspaces[URI].get()->addClient(socket, client);
    }

    qDebug() << "Numero Workspace: " << workspaces.size();

    emit openFile(socket, URI);

    /*
    QByteArray data;
    BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageHeaderFile(URI, client->getUsername(),  workspaces[URI].get()->getClients())); //DA SISTEMARE CON VERO CREATORE
    BuilderMessage::MessageSendToClient(data, workspaces[URI].get()->getSharedFile());
    socket->sendBinaryMessage(data);

    qDebug()<< "<MAIN WINDOWS> voglio aggiornare lista client di tutti i connessi";
    for(QSharedPointer<Client> cl : workspaces[URI]->getClients()){
        if(cl->getUsername().compare(client->getUsername())!=0){
            QByteArray data;
            BuilderMessage::MessageSendToClient(data,BuilderMessage::MessageInsertClientWorkspace(client->getUser()->getUsername(),client->getUser()->getNickname(),client->getUser()->getIcon())); //DA SISTEMARE CON VERO CREATORE
            cl->getSocket()->sendBinaryMessage(data);
        }
        qDebug()<<data;
    }
    */
}

void Tasks::serverInsertionChar(){
/*
    QString caller =clients[clientSocket]->getUsername();

    workspaces[QString::fromStdString(s.getSite())].get()->insertChar(s, "server", clientSocket, caller);

    QSharedPointer<Workspace> w =  workspaces[(s.getSite())];

    SharedFile *sf = w->getSharedFile();

    //AREA DEBUG
    qDebug() << "INSERIMENTO DA CLIENT";
    qDebug() << "File pre inserimento: " <<(sf->to_string());
    for(Symbol s : sf->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
    }
    //fine AREA DEBUG

    int esito = sf->localInsert(s, "server");
    qDebug()<< "Conflitto: "<<esito;
    if(esito==1){//conflitto
        qDebug()<<"Sono nella zona di conflitto perchè esito="<<esito;
        std::vector<Symbol> simboli = sf->getSymbols();
        std::vector<int> nuovapos;
        for(int i=0; i<simboli.size(); i++)
            if(simboli[i].getId()==s.getId()){
                nuovapos=simboli[i].getPosFraz();
                break;
            }
        Symbol news(s.getCar(),s.getSite(), nuovapos, s.getId(),s.getFmt());
        QByteArray data1;
        BuilderMessage::MessageSendToClient(
                    data1,BuilderMessage::MessageDelete(s.getCar(), s.getPosFraz(), (s.getId()), (s.getSite())));

        clientSocket->sendBinaryMessage(data1);
        for(QSharedPointer<Client> cl : w->getClients()){
                QByteArray data;
                BuilderMessage::MessageSendToClient(
                */
                      //      data,BuilderMessage::MessageInsert(s.getCar(), news.getPosFraz(), (s.getId()), (s.getSite())/*,"no"*/));
/*
                cl->getSocket()->sendBinaryMessage(data);
        }
    }
    else{
        qDebug()<< "E ARRIVATO IL MOMENTO DI MANDARE : ";

        for(QSharedPointer<Client> cl : w->getClients()){
            qDebug()<< "NUMERO ITERAZIONE ANCORA:";
            if(cl->getUsername()!=caller){
                qDebug()<< "STO MANDANDO "<< cl->getUsername();

                QByteArray data;
                BuilderMessage::MessageSendToClient(
                */
                            //data,BuilderMessage::MessageInsert(s.getCar(), s.getPosFraz(), s.getId(), s.getSite()/*,"no"*/));
/*
                cl->getSocket()->sendBinaryMessage(data);
            }
        }
    }
    //AREA DEBUG
    qDebug() << "File post inserimento: " << sf->to_string();
    for(Symbol s : sf->getSymbols()){
        qDebug() << s.getCar() << " con posfraz=" << s.getPosFraz() ;
    }
    //fine AREA DEBUG
    */
}
void Tasks::serverDeletionChar(){
    /*
    QString caller =clients[clientSocket]->getUsername();
    //workspaces[QString::fromStdString(s.getSite())].get()->localErase(s, "server", caller);

    QSharedPointer<Workspace> w =  workspaces[(s.getSite())];
    SharedFile *sf = w->getSharedFile();

    //AREA DEBUG
        qDebug() << "CANCELLAZIONE DA CLIENT";
        qDebug() << "File prima: " << sf->to_string();
        for(Symbol s1 : sf->getSymbols()){
            qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz()<<" e id="<<s1.getId();
        }
        //fine AREA DEBUG
    qDebug()<<"locla erase:"<<s.getPosFraz();

    sf->localErase(s);
    //AREA DEBUG
    qDebug()<<"Sto cancellando car="<<s.getCar()<< " con id="<<s.getId();
        qDebug() << "File dopo: " << sf->to_string();
        for(Symbol s1 : sf->getSymbols()){
            qDebug() << s1.getCar() << " con posfraz=" << s1.getPosFraz()<<" e id="<<(s1.getId());
        }
        //fine AREA DEBUG

    //INFORMA TUTTI I CLIENT TRANNE IL CHIAMANTE DELLA CANCELLAZIONE
    for(QSharedPointer<Client> cl : w->getClients()){
        if(cl->getUsername()!=caller){
            qDebug()<<"Sto informando della cancellazione client: "<<cl->getUsername();
            QByteArray data;
            BuilderMessage::MessageSendToClient(
                        data,BuilderMessage::MessageDelete(s.getCar(), s.getPosFraz(), s.getId(), s.getSite()));

            cl->getSocket()->sendBinaryMessage(data);
        }
    }
    */
}


void Tasks::serverRemoveClientFromWorkspace(){
    qDebug() << "ricevuto segnale di remove client from workspace";
    QString URI = request["fileName"].toString();

    QString caller = clients[socket]->getUsername();
    Workspace *w = workspaces[URI].get();

    w->removeClient(socket);
    qDebug() << "OK rimosso client,  n client: " << w->getClients().size() << "In workspace: " << URI;

    if(w->getClients().size()<=0){

        //Safe way to save a file. All the operation are done on a temporary unique-random named file.
        //Only if the commit() return true the original file is modified
        QSaveFile file(rootPath + URI);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
            file.resize(0);
            QTextStream out(&file);
            file.commit();
        }

        this->workspaces.remove(URI);

        qDebug() << "Rimosso workspace, numero workspaces: " << workspaces.size();
    }else{
        emit removeClientFromWorkspace(socket, URI); // Remove the current client from the other client still sharing the <fileName> file

        /*)
        for(auto cl : w->getClients()){
            qDebug() << "Mi devo rimuovere dagli altri";
            if(cl->getUsername()!=caller){
                QByteArray data;
                BuilderMessage::MessageSendToClient(
                            data,BuilderMessage::MessageRemoveClientFromWorkspace(caller));
                cl->getSocket()->sendBinaryMessage(data);
            }
        }
        */
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
        db.open(defaultnamedb,QString::number(QRandomGenerator::global()->generate()),ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        return;

    }

    try{
        if(db.addDocToUser(userToShare, URI)){
            for(auto client : clients.values()){
                if(client->getUsername() == userToShare){
                    //emit requestShareFile(client->getSocket(), URI, client->getUsername()); // In future, push request if client connected asking if accept or decline share request
                    this->serverOpenDirOfClient(client->getSocket());
                    break;
                }
            }
        }

    }catch (DatabaseNotFoundException& re ) {
        emit printUiServer(re.what());
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        return;
    }



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
        case TypeOperation::DeletionChar:               this->serverDeletionChar();                   break;
        case TypeOperation::RemoveClientFromWorkspace:  this->serverRemoveClientFromWorkspace();    break;
        case TypeOperation::ShareFile:                  this->serverShareFile();                    break;

        default: qDebug() << "No Task";
    }
}

Tasks::~Tasks()
{
    //Sistema
    if(QSqlDatabase::database(threadId).isValid())
        QSqlDatabase::removeDatabase(threadId);
    qDebug() << "curr thread  " << QThread::currentThread();
    qDebug() << "Returning to main thread  " << this->thread();
}
