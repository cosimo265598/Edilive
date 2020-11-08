
#include "serverdatabase.h"
#include "userdata.h"
#include <QFile>
#include <QVariant>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <serverexception.h>
#include <QRandomGenerator>


void ServerDatabase::open(QString dbName, QString connectionName, Ui::MainWindow* ui)
{
    this->ui_of_server=ui;


    this->db = QSqlDatabase::database(connectionName);

    if(!this->db.isValid()){
        this->db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        if(!this->db.isValid())
            throw DatabaseConnectionException(db.lastError());

        db.setDatabaseName(dbName);

    }
    if(ui!=nullptr)
        QObject::connect(this,&ServerDatabase::printUiServerDatabase,
                     ui->commetdb,&QPlainTextEdit::appendPlainText,Qt::QueuedConnection);

    if (!QFile::exists(dbName))
    {
        qDebug() << "QUI";

        // Database initialization queries

        QSqlQuery userTable(QSqlDatabase::database(connectionName));								// Query to create the Users table
        if (!userTable.exec(usertable) )
        {
            emit printUiServerDatabase("----- ERORRE creazione tabella USER "+db.lastError().text());
            throw DatabaseCreateException(userTable.lastQuery().toStdString(), userTable.lastError());
        }

        QSqlQuery docTable(QSqlDatabase::database(connectionName));								  // Query to create the DocEditors table
        if (!docTable.exec(documentable))
        {
            emit printUiServerDatabase("----- ERORRE creazione tabella FileName "+db.lastError().text());
            throw DatabaseCreateException(docTable.lastQuery().toStdString(), docTable.lastError());

        }
        QSqlQuery docUsertable(QSqlDatabase::database(connectionName));								  // Query to create the DocEditors table
        if (!docTable.exec(docusertable))
        {
            emit printUiServerDatabase("----- ERORRE creazione tabella DocUser "+db.lastError().text());
            throw DatabaseCreateException(docTable.lastQuery().toStdString(), docTable.lastError());

        }
    }

    if (!db.open()) {
        qDebug()<<LOG_PRINT_DB+"----- ERORRE apertura db esistente"+db.lastError().text();
        emit printUiServerDatabase("----- ERORRE apertura db esistente"+db.lastError().text());
        throw DatabaseConnectionException(db.lastError());
    }

    emit printUiServerDatabase("Ready for new connection - connection name: "+connectionName);

}

void ServerDatabase::insertNewDoc(file_t file)
{
    emit printUiServerDatabase("Query insertNewDoc");

    if(db.transaction()){

        QSqlQuery query(db);

        query.prepare("INSERT INTO Files (URI, FileName, Creator, Created) "
                              "VALUES (:uri, :filename, :creator, :created)");

        query.bindValue(":uri", file.URI);
        query.bindValue(":filename", file.fileName);
        query.bindValue(":creator", file.creator);
        query.bindValue(":created", file.created);

        if (!query.exec()){
            qDebug()<<LOG_PRINT_DB+"Error insert new Doc "+query.lastError().text();
            db.rollback();
            throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }


        query.prepare("INSERT INTO DocEditors (Username, URI, Pending) VALUES (:username, :uri, :pending)");
        query.bindValue(":username", file.creator);
        query.bindValue(":uri", file.URI);
        query.bindValue(":pending", 0);  // If 0, access allowed. If 1 means a shared file to accept or not

        if (!query.exec()){
            qDebug()<<LOG_PRINT_DB+"Error add doc to user";
            db.rollback();
            throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }

        if(!db.commit())
        {
             qDebug() << "Failed to commit";
             db.rollback();
             qDebug()<<LOG_PRINT_DB+"Error remove doc to user";
             throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }

    }else{
        qDebug() <<  "Failed to start transaction mode";
        throw DatabaseTransactionException("Failed to start transaction mode");
    }
}

ServerDatabase::ServerDatabase()
{
}

ServerDatabase::~ServerDatabase()
{
}


void ServerDatabase::insertUser(UserData& user)
{
    emit printUiServerDatabase("Query insertUser");

    int userId = -1;

    if(db.transaction()){
        QSqlQuery query(db);

        // Selection query for one user whit a particular username
        query.prepare("SELECT * FROM Users WHERE Username = :username");
        query.bindValue(":username", user.getUsername());

        if (!query.exec()){
            qDebug()<<LOG_PRINT_DB+"Error selecting existing user";
            throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
        }

        if(query.first()){
            qDebug()<<LOG_PRINT_DB+"Username already existing";
            throw DatabaseDuplicatedException(query.lastQuery().toStdString(), query.lastError());
        }

        if(query.exec("SELECT MAX(UserID) FROM Users"))
        {
            // Get the max value of the UserID column and return it incremented by 1 (first available ID)
            query.next();
            if (query.isValid()){
                userId = query.value(0).toInt() + 1;
                user.setUserId(userId);
            }else{
                qDebug()<<LOG_PRINT_DB+"Error get max user id";
                throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
            }
        }
        else
        {
            qDebug()<<LOG_PRINT_DB+"Error get max user id";
            throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
        }

        // Insertion query of a new record in the Users table
        query.prepare("INSERT INTO Users (Username, UserID, Nickname, PassHash, Salt, Icon) "
            "VALUES (:username, :id, :nickname, :passhash, :salt, :icon)");

        query.bindValue(":username", user.getUsername());
        query.bindValue(":id", user.getUserId());
        query.bindValue(":nickname", user.getNickname());
        query.bindValue(":passhash", user.getPasswordHash());
        query.bindValue(":salt", user.getSalt());
        query.bindValue(":icon", user.getIcon());

        if (!query.exec()){
            qDebug()<<LOG_PRINT_DB+"Error insert new insert user "+query.lastError().text();
            db.rollback();
            throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }

        if(!db.commit())
        {
             qDebug() << "qui";
             qDebug() << "Failed to commit";
             db.rollback();
             qDebug()<<LOG_PRINT_DB+"Error insert new insert user "+query.lastError().text();
             throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }

    }else{
        qDebug() <<  "Failed to start transaction mode";
        throw DatabaseTransactionException("Failed to start transaction mode");
    }
}

void ServerDatabase::updateUser(const UserData& user)
{
    emit printUiServerDatabase("Query updateUser");

    QSqlQuery query(db);

    // Update query of an existing record in the Users table
    query.prepare("UPDATE Users SET Nickname = :nickname, PassHash = :passhash, Salt = :salt, Icon = :icon "
        "WHERE Username = :username");

    QByteArray icon;
    QDataStream out(&icon, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_13);
    out << user.getIcon();

    query.bindValue(":username", user.getUsername());
    query.bindValue(":nickname", user.getNickname());
    query.bindValue(":passhash", user.getPasswordHash());
    query.bindValue(":salt",     user.getSalt());
    query.bindValue(":icon",     icon);

    if (!query.exec()){
        qDebug()<<LOG_PRINT_DB+"Error update user";
        throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
    }
}

bool ServerDatabase::addDocToUser(QString username, QString URI)
{
    emit printUiServerDatabase("Query addDocToUser");

    bool exists = false;

    if(db.transaction()){

        QSqlQuery query(db);

        // Selection query for one user whit a particular username
        query.prepare("SELECT * FROM Users WHERE Username = :username");
        query.bindValue(":username", username);
        if (!query.exec()){
            qDebug()<<LOG_PRINT_DB+"Error select existing user";
            db.rollback();
            throw DatabaseNotFoundException(query.lastQuery().toStdString(), query.lastError());
        }

        if(query.first()){
            query.prepare("INSERT INTO DocEditors (Username, URI, Pending) VALUES (:username, :uri, :pending)");

            query.bindValue(":username", username);
            query.bindValue(":uri", URI);
            query.bindValue(":pending", 0); // In future, put here 1 in order to allow the destinator to choose if accept or not the shared file

            if (!query.exec()){
                qDebug()<<LOG_PRINT_DB+"Error add doc to user";
                throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
            }

            exists = true;
        }

        if(!db.commit())
        {
             qDebug() << "Failed to commit";
             db.rollback();
             qDebug()<<LOG_PRINT_DB+"Error remove doc to user";
             throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }

        return exists;

    }else{
        qDebug() <<  "Failed to start transaction mode";
        throw DatabaseTransactionException("Failed to start transaction mode");
    }
}

bool ServerDatabase::removeDocFromUser(QString username, QString URI)
{
    bool cancel = false;
    if(db.transaction())
    {
        QSqlQuery query(db);
        // Deletion query of a User-filename pair from the DocEditors table
        query.prepare("DELETE FROM DocEditors WHERE Username = :username AND URI = :uri");

        query.bindValue(":username", username);
        query.bindValue(":uri", URI);
        if(!query.exec()){
            db.rollback();
            throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
        }


        query.prepare("SELECT COUNT(*) FROM DocEditors WHERE URI = :uri");
        query.bindValue(":uri", URI);
        if(!query.exec()){
            db.rollback();
            throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
        }

        query.next();
        if(!query.isValid()){
            db.rollback();
            throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
        }

        if(query.value(0).toInt()==0){
            query.prepare("DELETE FROM Files WHERE URI = :uri");
            query.bindValue(":uri", URI);

            if(!query.exec()){
                db.rollback();
                throw DatabaseDeleteException(query.lastQuery().toStdString(), query.lastError());
            }

            cancel = true;
        }

       if(!db.commit())
       {
            db.rollback();
            qDebug()<<LOG_PRINT_DB+"Failed to commit";
            throw DatabaseDeleteException(query.lastQuery().toStdString(), query.lastError());
       }

       return cancel;
    }
    else
    {
        qDebug() <<  "Failed to start transaction mode";
        throw DatabaseTransactionException("Failed to start transaction mode");
    }
}

void ServerDatabase::removeDoc(QString URI)
{
    emit printUiServerDatabase("Query removeDoc");

    QSqlQuery query(db);

    // Deletion query of all records referring to a document filename in the DocEditors table
    query.prepare("DELETE FROM Files WHERE URI = :uri");
    query.bindValue(":uri", URI);


    if (!query.exec()){
        qDebug()<<LOG_PRINT_DB+"Error remove doc ";
        throw DatabaseWriteException(query.lastQuery().toStdString(), query.lastError());
    }
}

void ServerDatabase::removeDatabase(QString connectionName)
{
    this->db.close();
    //this->db.removeDatabase(connectionName);
}


int ServerDatabase::getMaxUserID()
{
    emit printUiServerDatabase("Query getMaxUserID");

    QSqlQuery query(db);
    if (query.exec("SELECT MAX(UserID) FROM Users") && query.isActive())
    {
        // Get the max value of the UserID column and return it incremented by 1 (first available ID)
        query.next();
        if (query.isValid())
            return query.value(0).toInt() + 1;
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error get max user id";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());

    }

    return 0;
}

QList<UserData> ServerDatabase::readUsersList()
{
    emit printUiServerDatabase("Query readUsersList");
    QList<UserData> users;
    QSqlQuery query(db);

    if (query.exec("SELECT * FROM Users") && query.isActive())
    {
        // Read all the users' information from the database and load them in memory
        query.next();
        while (query.isValid())
        {
            UserData user(query.value("Username").toString(),
                query.value("UserID").toInt(),
                query.value("Nickname").toString(),
                query.value("PassHash").toByteArray(),
                query.value("Salt").toByteArray(),
                query.value("Icon").toByteArray());

            users.append(user);

            query.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read user list";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());

    }

    return users;
}

UserData ServerDatabase::readUser(QString username)
{
    emit printUiServerDatabase("Query readUser");

    QSqlQuery query(db);

    // Selection query for one user whit a particular username
    query.prepare("SELECT * FROM Users WHERE Username = :username");
    query.bindValue(":username", username);

    if (!query.exec()){
        qDebug()<<LOG_PRINT_DB+"Error select existing user";

        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
    }

    if(!query.first()){
        qDebug() << query.lastQuery()<< "Seconda parte: " << query.lastError();
        throw DatabaseNotFoundException(query.lastQuery().toStdString(), query.lastError());
    }

    QByteArray b = query.value("Icon").toByteArray();

    return UserData(
                    query.value("Username").toString(),
                    query.value("UserID").toInt(),
                    query.value("Nickname").toString(),
                    query.value("PassHash").toByteArray(),
                    query.value("Salt").toByteArray(),
                    query.value("Icon").toByteArray()
     );
}

QStringList ServerDatabase::readUserDocuments(QString username)
{
    emit printUiServerDatabase("Query readUsersDocuments");

    QSqlQuery query(db);
    // Selection query of all the filenames owned by a user (DocEditors table)
    query.prepare("SELECT Files.FileName, Files.Creator, Files.Created, Files.URI FROM Files LEFT JOIN DocEditors WHERE Files.URI = DocEditors.URI AND Username = :username AND Pending = :pending ORDER BY Files.Filename ASC");

    query.bindValue(":username", username);
    query.bindValue(":pending", 0);

    QStringList docs;

    if (query.exec() && query.isActive())
    {
        query.next();
        while (query.isValid())
        {
            docs << query.value(0).toString();
            query.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read user documents ";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
    }

    return docs;
}

QStringList ServerDatabase::readDocuments()
{
    emit printUiServerDatabase("Query readDocumentfilename");

    QList<QString> documents;
    QSqlQuery query(db);
    if (query.exec("SELECT DISTINCT FileName FROM DocEditors") && query.isActive())
    {
        // Load all the document filenames in a QString list
        query.next();
        while (query.isValid())
        {
            documents.append(query.value(0).toString());
            query.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read coument filename";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());

    }

    return documents;
}

int ServerDatabase::countDocEditors(QString URI)
{
    emit printUiServerDatabase("Query countDocEditor");

    QSqlQuery qCountDocEditors(db);

    // Query to count the editors of a document (DocEditors table)
    qCountDocEditors.prepare("SELECT COUNT(*) FROM DocEditors WHERE URI = :uri");

    qCountDocEditors.bindValue(":uri", URI);

    if (qCountDocEditors.exec() && qCountDocEditors.isActive())
    {
        qCountDocEditors.next();
        return qCountDocEditors.value(0).toInt();
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error count doc editor";
        throw DatabaseReadException(qCountDocEditors.lastQuery().toStdString(), qCountDocEditors.lastError());

    }
    return 0;

}

QList<file_t> ServerDatabase::getUserDocs(QString username)
{
    emit printUiServerDatabase("Query getUserDocs");
    QList<file_t>  files;

    QSqlQuery query(db);
    // Selection query of all the filenames owned by a user (DocEditors table)
    query.prepare("SELECT Files.FileName, Files.Creator, Files.Created, Files.URI FROM Files LEFT JOIN DocEditors WHERE Files.URI = DocEditors.URI AND Username = :username AND Pending = :pending");

    query.bindValue(":username", username);
    query.bindValue(":pending", 0);

    if (query.exec() && query.isActive())
    {

        query.next();
        // Read all the users' information from the database and load them in memory
        while (query.isValid())
        {
            file_t file;
            file.URI = query.value("URI").toString(),
            file.fileName = query.value("FileName").toString(),
            file.creator = query.value("Creator").toString(),
            file.created = query.value("Created").toString(),

            files.append(file);
            query.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read user list";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());
    }

    return files;
}
