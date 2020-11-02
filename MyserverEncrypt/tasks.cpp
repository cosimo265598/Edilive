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


    qDebug()<<username<<" Server login request  -  "<<this->thread()->currentThread()<< threadId;
    //QThread::currentThread()->msleep(10000);

    try {
        database.open(defaultnamedb,threadId,ui);
        UserData user(database.readUser(username));

        if(user.isEmpty()){            // utente non registrato, non presente nel db.
            emit printUiServer("Client not Registered "+username+ " served by Thread: "+threadId);
            emit loginError(socket, "Client not registered." + username);
            emit socketAbort(socket);
            return;
        }else{
            for (QString docUri : database.readUserDocuments(user.getUsername()))
                user.addDocument(docUri);
            users.insert(user.getUsername(), user);
        }

    }  catch (DatabaseReadException& re) {
        users.remove(username);
        emit printUiServer("Database problem during the query excution served by Thread: "+threadId);
        emit loginError(socket, "Error database sever");
        emit socketAbort(socket);
        return;
    }
    if (client.get()->isLogged()){
        emit printUiServer("Client already logged in as '" + client->getUsername() + "'");
        emit loginError(socket, "Client: "+client->getUsername()+" is already logged in.");
        return;
    }

    // client not logged
    emit messageChallenge(socket, QString(users[username].getSalt()), QString(client->challenge(&users[username])));

}

void Tasks::serverLoginUnlock()
{
    this->threadId = QString::number((quintptr)QThread::currentThreadId());
    QString token = request.value("challange").toString();
    QSharedPointer<Client> client = clients[socket];

    QByteArray data;

    if (client->isLogged()){
        emit loginError(socket, "You have to loggin before use the platform");
        return ;

    }else if (client->authenticate(token.toUtf8())){		// verify the user's account credentials
        emit printUiServer("User " + client->getUsername() + " is logged in -  served by Thread: "+threadId);
        client->login(client->getUser());
        emit messageChallegePassed(socket, QString());
        return ;
    }else{
        users.remove(client->getUsername());
        client->logout();
        emit loginError(socket, "Credential inserted are not corrected");
        emit socketAbort(socket);

        return ;
    }
}

void Tasks::serverAccountCreate()
{
    ServerDatabase database;
    QString username = request["username"].toString();
    QString password = request["password"].toString();
    this->threadId = QString::number((quintptr)QThread::currentThreadId());

    QSharedPointer<Client> client = clients[socket];

    //check if this username is already used
    try {

        database.open(defaultnamedb,threadId,ui);

        UserData user(database.readUser(username));
        if(!user.isEmpty()){
            emit accountCreationError(socket, "Username already exist");
            return;
        }
    }  catch (DatabaseReadException& re ) {
        emit printUiServer("Databaseread problem during the query execution");
        emit accountCreationError(socket, "Error database sever during registration phase");
        emit socketAbort(socket);

        return;
    }
    // no check on image because are used for the firts time default settings

    emit printUiServer("Creating new user account: "+username);
    int userId = database.getMaxUserID();

    UserData user(username, userId++, "nickname", password, QImage(":/images/default.png"));		/* create a new user		*/
    users.insert(username, user);               /* insert new user in map	*/
    qDebug() << users[username].getNickname() << " " << users[username].getUserId() << " " ;

    try
    {	// Add the new user record to the server database
        database.insertUser(user);
        if (!QDir(QDir().currentPath()+"/Users").mkdir(username)) {
            emit printUiServer("Cannot create folder for Users: "+ client->getUsername());
            throw DatabaseCreateException("Can not create folder for new user",QSqlError());
        }
        // inserimento di un file di prova
        bool state=QFile::copy(QDir().currentPath()+"/examplefile.html",
                    QDir().currentPath()+"/Users/"+username+"/examplefile");
        if(state)
            throw DatabaseCreateException("Can not copy file example for new user",QSqlError());

    }catch (DatabaseException& dbe) {
        emit printUiServer(dbe.what());
        client->logout();
        users.remove(username);
        QDir(QDir().currentPath()+"/Users").rmdir(client->getUsername());
        emit accountCreationError(socket, "Internal Error");
        emit socketAbort(socket);
        return;
    }

    emit accountConfirmed(socket, "Account Created Correctly");
    return;

}

void Tasks::serverOpenDirOfClient(QWebSocket* pushSocket)
{
    /*
    QSharedPointer<Client> client = clients[socket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername());
    QDirIterator it(path, QDir::Files, QDirIterator::NoIteratorFlags);
    QJsonArray files;

    // visita della directory
    while (it.hasNext()) {
        it.next();
        files.append(QJsonObject{
                         {"filename", it.fileInfo().fileName()}, // meglio file name
                         {"owner", it.fileInfo().owner()},
                         {"lastModified",  it.fileInfo().lastModified().toString()},
                         {"lastRead", it.fileInfo().lastRead().toString()},
                         {"size", QString::number(it.fileInfo().size()) }
                     });
    }

    emit openDirOfClient(socket, files);
    */

    QSharedPointer<Client> client = clients[socket];
    QJsonArray files;
    QList<file_t> fileList;

    ServerDatabase db;

    try {
        db.open(defaultnamedb,threadId,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error creation file");
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error creation file");
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

        files.append(QJsonObject{
                         {"filename", f.fileName}, // meglio file name
                         {"creator", f.creator},
                         {"created",  f.created},
                         {"URI", f.URI},
                         {"size", QFileInfo(rootPath+f.URI).size()}
                     });
    }

    if(pushSocket != nullptr)
        emit openDirOfClient(pushSocket, files);
    else
        emit openDirOfClient(socket, files);
}

void Tasks:: serverUpdateProfileClient(){
    qDebug()<<"segnale di modifica dati profilo ricevuto";

    ServerDatabase database;

    QString nickname = request["nickname"].toString();
    QString password = request["password"].toString();
    QString stringifiedImage = request["image"].toString();

    QSharedPointer<Client> client = clients[socket];
    QByteArray data;
    QImage image;
    image.loadFromData(QByteArray::fromBase64(stringifiedImage.toLatin1()));

    UserData user = users[client->getUsername()];
    user.update(nickname, password, image);

    try {

        database.open(defaultnamedb,threadId,ui);

        //UserData user(database.readUser(client->getUsername()));
        /*
        if(user.isEmpty()){
            emit accountCreationError(socket, "Username already exist");
            return;
        }
        */

    }  catch (DatabaseReadException& re ) {
        emit printUiServer("Databaseread problem during the query execution");
        emit accountCreationError(socket, "Error database sever during registration phase");
        emit socketAbort(socket);

        return;
    }


    try
    {	// Add the new user record to the server database
        database.updateUser(user);

    }catch (DatabaseException& dbe) {
        emit printUiServer(dbe.what());
        emit accountUpdateError(socket, "Update Error");
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

    QString fileName = request["nomefile"].toString();
    QSharedPointer<Client> client = clients[socket];

    QString URI = client->getUsername()+"_"+fileName;
    QString filePath = rootPath + URI;
    QFile newFile(filePath);

    qDebug() << newFile;

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
        db.open(defaultnamedb,threadId,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(filePath).remove();
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(filePath).remove();
        return;
    }


    try{
        db.insertNewDoc(URI,fileName,client->getUsername(), QFileInfo(newFile).created().timeZoneAbbreviation());
        this->serverOpenDirOfClient(nullptr);

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error deletion file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(filePath).remove();
        return;
    }

    /*
    QString fileName = request["nomefile"].toString();
    QSharedPointer<Client> client = clients[socket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QByteArray data;

    QFile filecreate(path);
    if(filecreate.exists()){
        qDebug()<< " File già presente- "<<fileName;
        emit fileCreationError(socket, "File already exists");
        return;
    }
    if (filecreate.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&filecreate);
        out << "";
        filecreate.close();
        //fileCreationSuccess(clientSocket, path);
        this->serverOpenDirOfClient();

    }else{
        emit fileCreationError(socket, "Error creation the new file.");
    }
    */
}

void Tasks::serverDeleteFileForClient()
{
    /*
    qDebug()<<"segnale delete file ricevuto";

    QString fileName = request["nomefile"].toString();
    QSharedPointer<Client> client = clients[socket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QFile file(path);
    if(!file.exists()){
        qDebug()<< "File not found";
        emit fileDeletionError(socket, "File Not Found");

        return;
    }else{
        file.remove();
        this->serverOpenDirOfClient();
    }
    */

    qDebug()<<"segnale delete file ricevuto";

    QString URI = request["nomefile"].toString();
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
        db.open(defaultnamedb,threadId,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        emit fileDeletionError(socket, "Error deletion file");
        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileDeletionError(socket, "Error deletion file");
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

    /*


    db.insertNewDoc(URI,fileName,client->getUsername(), QFileInfo(newFile).created().toString());
    qDebug() << "ora seconda";
    db.addDocToUser(client->getUsername(), URI);
    qDebug() << "ok seconda";
    this->serverOpenDirOfClient();

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(filePath).remove();
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        emit fileCreationError(socket, "Error creation file");
        QFile(URI).remove();
        return;
    }

     */
}

void Tasks::serverOpenFile()
{
    /*
    qDebug()<<"Segnale apertura file ricevuto";
    QString fileName = request["nomefile"].toString();

    QSharedPointer<Client> client = clients[socket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    QFile filecreate(path);
    if(!QFile(path).exists()){
       return; // Messaggio errore file not found o qualcosa del genere
    }

    // OPERAZIONI SUL WORKSPACE, MI UNISCO AD UN WORKSPACE OPPURE LO CREO E MI UNISCO COME PRIMO CLIENT
    //Al posto del fileName mettere docURI

    if(!workspaces.contains(fileName)){
        //QSharedPointer<Workspace> workspace(new Workspace(nullptr, fileName));
        workspaces.insert(fileName, QSharedPointer<Workspace>(new Workspace(nullptr, path)));
        workspaces[fileName].get()->addClient(socket, client);

    }
    else{
        qDebug() << "Adding current client to existing workspace";
        workspaces[fileName].get()->addClient(socket, client);
    }

    qDebug() << "Numero Workspace: " << workspaces.size();

    emit openFile(socket, fileName);
    */

    qDebug()<<"Segnale apertura file ricevuto";
    QString fileName = request["nomefile"].toString();

    QSharedPointer<Client> client = clients[socket];
    QString filePath = rootPath + fileName;
    QFile filecreate(filePath);
    if(!QFile(filePath).exists()){
       return; // Messaggio errore file not found o qualcosa del genere
    }

    // OPERAZIONI SUL WORKSPACE, MI UNISCO AD UN WORKSPACE OPPURE LO CREO E MI UNISCO COME PRIMO CLIENT
    //Al posto del fileName mettere docURI

    qDebug() << fileName;

    if(!workspaces.contains(fileName)){
        //QSharedPointer<Workspace> workspace(new Workspace(nullptr, fileName));
        workspaces.insert(fileName, QSharedPointer<Workspace>(new Workspace(nullptr, filePath)));
        workspaces[fileName].get()->addClient(socket, client);

    }
    else{
        qDebug() << "Adding current client to existing workspace";
        workspaces[fileName].get()->addClient(socket, client);
    }

    qDebug() << "Numero Workspace: " << workspaces.size();

    emit openFile(socket, fileName);
}

void Tasks::insertionChar(QWebSocket *clientSocket, Symbol s){

    QString caller =clients[clientSocket]->getUsername();
    /*
    workspaces[QString::fromStdString(s.getSite())].get()->insertChar(s, "server", clientSocket, caller);
    */
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
                            data,BuilderMessage::MessageInsert(s.getCar(), news.getPosFraz(), (s.getId()), (s.getSite())/*,"no"*/));

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
                            data,BuilderMessage::MessageInsert(s.getCar(), s.getPosFraz(), s.getId(), s.getSite()/*,"no"*/));

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
}
void Tasks::deletionChar(QWebSocket *clientSocket, Symbol s){
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
}


void Tasks::serverRemoveClientFromWorkspace(){
    qDebug() << "ricevuto segnale di remove client from workspace";
    QString URI = request["fileName"].toString();

    qDebug() << URI;
    workspaces[URI]->removeClient(socket);
    qDebug() << "OK rimosso client,  n client: " << workspaces[URI]->getClients().size() << "In workspace: " << URI;

    if(workspaces[URI]->getClients().size()<=0){

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
    }
    else
        emit removeClientFromWorkspace(socket, URI); // Remove the current client from the other client still sharing the <fileName> file
}

void Tasks::serverShareFile()
{
    qDebug() << "ricevuto segnale shareFile";

    QString URI = request["URI"].toString();
    QString userToShare = request["username"].toString();

    QSharedPointer<Client> client = clients[socket];

    ServerDatabase db;

    try {
        db.open(defaultnamedb,threadId,ui);
    }catch (DatabaseConnectionException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error creation file");

        return;
    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error creation file");

        return;
    }

    try{
        if(db.addDocToUser(userToShare, URI)){
            for(auto client : clients.values()){
                if(client->getUsername() == userToShare){
                    //emit requestShareFile(client->getSocket(), URI, client->getUsername()); // In future, push request if client connected
                    this->serverOpenDirOfClient(client->getSocket());
                    break;
                }
            }
        }

    }catch (DatabaseReadException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseWriteException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseTransactionException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error deletion file");
        return;

    }catch (DatabaseException& re ) {
        emit printUiServer(re.what());
        //emit fileCreationError(socket, "Error deletion file");
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
        case TypeOperation::DeleteChar:                 this->serverDeleteChar();                   break;
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
