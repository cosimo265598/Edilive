#include "tasks.h"

Tasks::Tasks(QObject *parent,
             QJsonObject request, QWebSocket* socket, QMap<QWebSocket*,
             QSharedPointer<Client>>& clients, QMap<QString, UserData>& users,
             TypeOperation typeOp,Ui::MainWindow *ui) :
    QObject(parent),
    clients(clients),
    users(users),
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

void Tasks::serverOpenDirOfClient()
{
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
}

void Tasks:: serverUpdateProfileClient(){
    qDebug()<<"segnale di modifica dati profilo ricevuto";

    QString nickname = request["nickname"].toString();
    QString password = request["password"].toString();
    QString stringifiedImage = request["image"].toString();

    QSharedPointer<Client> client = clients[socket];
    QByteArray data;
    QImage image;
    image.loadFromData(QByteArray::fromBase64(stringifiedImage.toLatin1()));

    users[client->getUsername()].update(nickname, password, image);

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
}

void Tasks::serverDeleteFileForClient()
{
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
}

void Tasks::serverOpenFile()
{
    qDebug()<<"Segnale apertura file ricevuto";
    QString fileName = request.value("nomefile").toString();
    QSharedPointer<Client> client = clients[socket];
    QString path(QDir().currentPath()+"/Users/"+client->getUsername()+"/"+fileName);

    qDebug()<<client->getUsername()<<" "<<path<<" file name "<<fileName;

    QByteArray contentFile = nullptr;
    QFile filecreate(path);
    if(filecreate.exists()){
        if (filecreate.open(QIODevice::ReadWrite)){
            contentFile = filecreate.readAll();
            filecreate.close();
        }
    }
    else
        return;

    emit openFile(socket, fileName, contentFile);
}


void Tasks::run(){
    switch (typeOp) {

        case TypeOperation::LoginRequest:   this->serverLoginRequest();               break;
        case TypeOperation::LoginUnlock:    this->serverLoginUnlock();                 break;
        case TypeOperation::AccountCreate:  this->serverAccountCreate();             break;
        case TypeOperation::OpenDirectory:  this->serverOpenDirOfClient();           break;
        case TypeOperation::ProfileData:    this->serverPersonalDataOfClient();        break;
        case TypeOperation::AccountUpdate:  this->serverUpdateProfileClient();       break;
        case TypeOperation::CreateFile:     this->serverCreateFileForClient();          break;
        case TypeOperation::DeleteFile:     this->serverDeleteFileForClient();          break;
        case TypeOperation::OpenFile:       this->serverOpenFile();                      break;

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
