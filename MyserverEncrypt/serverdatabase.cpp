#include "serverdatabase.h"
#include "userdata.h"
#include <QFile>
#include <QVariant>
#include <QImage>
#include <QByteArray>
#include <QBuffer>
#include <serverexception.h>


void ServerDatabase::open(QString dbName, Ui::MainWindow* ui)
{
    this->ui_of_server=ui;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    bool creatingDb=true;

    if(QFile::exists(dbName)){
        creatingDb=false;
        ui->commetdb->appendPlainText("----- Caricamento db esistente.");
    }
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug()<<LOG_PRINT_DB+"----- ERORRE apertura db esistente"+db.lastError().text();
        ui->commetdb->appendPlainText("----- ERORRE apertura db esistente"+db.lastError().text());
        throw DatabaseConnectionException(db.lastError());

    }
    if (creatingDb)
    {
        // Database initialization queries

        QSqlQuery userTable(db);								// Query to create the Users table
        if (!userTable.exec(usertable) )
        {
            ui->commetdb->appendPlainText("----- ERORRE creazione tabella USER "+db.lastError().text());
            throw DatabaseCreateException(userTable.lastQuery().toStdString(), userTable.lastError());
        }

        QSqlQuery docTable(db);								  // Query to create the DocEditors table
        if (!docTable.exec(documentable))
        {
            ui->commetdb->appendPlainText("----- ERORRE creazione tabella DOCURI "+db.lastError().text());
            throw DatabaseCreateException(docTable.lastQuery().toStdString(), docTable.lastError());

        }
    }

    // Prepare all var-arg queries, leave only parameter binding for later

    qInsertNewUser	 = QSqlQuery(db);
    qUpdateUser		 = QSqlQuery(db);
    qInsertDocEditor = QSqlQuery(db);
    qRemoveDocEditor = QSqlQuery(db);
    qRemoveDoc		 = QSqlQuery(db);
    qCountDocEditors = QSqlQuery(db);
    qSelectUserDocs	 = QSqlQuery(db);
    qExistUser       = QSqlQuery(db);

    // Insertion query of a new record in the Users table
    qInsertNewUser.prepare("INSERT INTO Users (Username, UserID, Nickname, PassHash, Salt, Icon) "
        "VALUES (:username, :id, :nickname, :passhash, :salt, :icon)");

    // Update query of an existing record in the Users table
    qUpdateUser.prepare("UPDATE Users SET Nickname = :nickname, PassHash = :passhash, Salt = :salt, Icon = :icon "
        "WHERE Username = :username");

    // Insertion query of a new User-URI pair in the DocEditors table
    qInsertDocEditor.prepare("INSERT INTO DocEditors (Username, DocURI) VALUES (:username, :uri)");

    // Deletion query of a User-URI pair from the DocEditors table
    qRemoveDocEditor.prepare("DELETE FROM DocEditors WHERE Username = :username AND DocURI = :uri");

    // Deletion query of all records referring to a document URI in the DocEditors table
    qRemoveDoc.prepare("DELETE FROM DocEditors WHERE DocURI = :uri");

    // Query to count the editors of a document (DocEditors table)
    qCountDocEditors.prepare("SELECT COUNT(*) FROM DocEditors WHERE DocURI = :uri");

    // Selection query of all the URIs owned by a user (DocEditors table)
    qSelectUserDocs.prepare("SELECT DocURI FROM DocEditors WHERE Username = :username");

    // Selection query for one user whit a particular username
    qExistUser.prepare("SELECT * FROM Users WHERE Username = :username");

    this->ui_of_server->commetdb->appendPlainText("Ready");

}

void ServerDatabase::insertUser(const UserData& user)
{
    this->ui_of_server->commetdb->appendPlainText("Query insertUser");

    QByteArray icon;
    QBuffer buffer(&icon);
    buffer.open(QIODevice::WriteOnly);
    user.getIcon().save(&buffer, "PNG");	// writes image into the bytearray in PNG format

    qInsertNewUser.bindValue(":username", user.getUsername());
    qInsertNewUser.bindValue(":id", user.getUserId());
    qInsertNewUser.bindValue(":nickname", user.getNickname());
    qInsertNewUser.bindValue(":passhash", user.getPasswordHash());
    qInsertNewUser.bindValue(":salt", user.getSalt());
    qInsertNewUser.bindValue(":icon", icon);

    if (!qInsertNewUser.exec()){
        qDebug()<<LOG_PRINT_DB+"Error insert user "+qInsertNewUser.lastError().text();
        throw DatabaseWriteException(qInsertNewUser.lastQuery().toStdString(), qInsertNewUser.lastError());

    }
}

void ServerDatabase::updateUser(const UserData& user)
{
    this->ui_of_server->commetdb->appendPlainText("Query updateUser");

    QByteArray icon;
    QBuffer buffer(&icon);
    buffer.open(QIODevice::WriteOnly);
    user.getIcon().save(&buffer, "PNG");	// writes image into the bytearray in PNG format

    qUpdateUser.bindValue(":username", user.getUsername());
    qUpdateUser.bindValue(":nickname", user.getNickname());
    qUpdateUser.bindValue(":passhash", user.getPasswordHash());
    qUpdateUser.bindValue(":salt",     user.getSalt());
    qUpdateUser.bindValue(":icon",     icon);

    if (!qUpdateUser.exec()){
        qDebug()<<LOG_PRINT_DB+"Error update user";
        throw DatabaseWriteException(qUpdateUser.lastQuery().toStdString(), qUpdateUser.lastError());
    }
}

void ServerDatabase::addDocToUser(QString username, QString uri)
{
    this->ui_of_server->commetdb->appendPlainText("Query addDocToUser");

    qInsertDocEditor.bindValue(":username", username);
    qInsertDocEditor.bindValue(":uri", uri);

    if (!qInsertDocEditor.exec()){
        qDebug()<<LOG_PRINT_DB+"Error add doc to user";
        throw DatabaseWriteException(qInsertDocEditor.lastQuery().toStdString(), qInsertDocEditor.lastError());
    }
}

void ServerDatabase::removeDocFromUser(QString username, QString uri)
{
    this->ui_of_server->commetdb->appendPlainText("Query removeDocFromUser");

    qRemoveDocEditor.bindValue(":username", username);
    qRemoveDocEditor.bindValue(":uri", uri);

    if (!qRemoveDocEditor.exec()){
        qDebug()<<LOG_PRINT_DB+"Error remove doc from  user";
        throw DatabaseWriteException(qInsertDocEditor.lastQuery().toStdString(), qInsertDocEditor.lastError());

    }
}

void ServerDatabase::removeDoc(QString uri)
{
    this->ui_of_server->commetdb->appendPlainText("Query removeDoc");

    qRemoveDoc.bindValue(":uri", uri);

    if (!qRemoveDoc.exec()){
        qDebug()<<LOG_PRINT_DB+"Error remove doc ";
        throw DatabaseWriteException(qRemoveDocEditor.lastQuery().toStdString(), qRemoveDocEditor.lastError());
    }
}


int ServerDatabase::getMaxUserID()
{
    this->ui_of_server->commetdb->appendPlainText("Query getMaxUserID");

    QSqlQuery query;
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
    this->ui_of_server->commetdb->appendPlainText("Query readUsersList");

    QList<UserData> users;
    QSqlQuery query;
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
                QImage::fromData(query.value("Icon").toByteArray()));

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
    this->ui_of_server->commetdb->appendPlainText("Query readUser");

    qExistUser.bindValue(":username", username);

    if (!qExistUser.exec()){
        qDebug()<<LOG_PRINT_DB+"Error read user";
        throw DatabaseReadException(qExistUser.lastQuery().toStdString(), qExistUser.lastError());
    }
    if(qExistUser.first()){
        UserData user(qExistUser.value("Username").toString(),
        qExistUser.value("UserID").toInt(),
        qExistUser.value("Nickname").toString(),
        qExistUser.value("PassHash").toByteArray(),
        qExistUser.value("Salt").toByteArray(),
        QImage::fromData(qExistUser.value("Icon").toByteArray()));
        return user;
    }

    return UserData(); // return an empty data

}

QStringList ServerDatabase::readUserDocuments(QString username)
{
    this->ui_of_server->commetdb->appendPlainText("Query readUsersDocuments");

    QStringList docs;
    qSelectUserDocs.bindValue(":username", username);

    if (qSelectUserDocs.exec() && qSelectUserDocs.isActive())
    {
        qSelectUserDocs.next();
        while (qSelectUserDocs.isValid())
        {
            docs << qSelectUserDocs.value(0).toString();
            qSelectUserDocs.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read user documents ";
        throw DatabaseReadException(qSelectUserDocs.lastQuery().toStdString(), qSelectUserDocs.lastError());


    }

    return docs;
}

QStringList ServerDatabase::readDocumentURIs()
{
    this->ui_of_server->commetdb->appendPlainText("Query readDocumentURI");

    QList<QString> documents;
    QSqlQuery query;
    if (query.exec("SELECT DISTINCT DocURI FROM DocEditors") && query.isActive())
    {
        // Load all the document URIs in a QString list
        query.next();
        while (query.isValid())
        {
            documents.append(query.value(0).toString());
            query.next();
        }
    }
    else
    {
        qDebug()<<LOG_PRINT_DB+"Error read coument uri";
        throw DatabaseReadException(query.lastQuery().toStdString(), query.lastError());

    }

    return documents;
}

int ServerDatabase::countDocEditors(QString docURI)
{
    this->ui_of_server->commetdb->appendPlainText("Query countDocEditor");

    qCountDocEditors.bindValue(":uri", docURI);

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
