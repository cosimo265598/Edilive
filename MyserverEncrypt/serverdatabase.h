#ifndef SERVERDATABASE_H
#define SERVERDATABASE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "userdata.h"
#include <QDateTime>
#include "ui_mainwindow.h"


#define usertable "CREATE TABLE \"Users\" (\
                    \"Username\"	TEXT UNIQUE,\
                    \"UserID\"	INTEGER NOT NULL UNIQUE,\
                    \"Nickname\"	TEXT,\
                    \"PassHash\"	BLOB NOT NULL,\
                    \"Salt\"	BLOB NOT NULL,\
                    \"Icon\"	BLOB,\
                    PRIMARY KEY(\"Username\"));"


#define documentable "CREATE TABLE \"DocEditors\" (\
                     \"Username\"	TEXT NOT NULL,\
                     \"DocURI\"	TEXT NOT NULL,\
                     FOREIGN KEY(\"Username\") REFERENCES \"Users\"(\"Username\") ON UPDATE CASCADE ON DELETE CASCADE,\
                     PRIMARY KEY(\"Username\", \"DocURI\"));"

#define defaultnamedb "sql_for_server"

#define LOG_PRINT_DB  QDateTime::currentDateTime().toString()


class ServerDatabase
{
private:
    Ui::MainWindow *ui_of_server;
    // Prepared queries
    QSqlQuery qInsertNewUser;
    QSqlQuery qUpdateUser;
    QSqlQuery qInsertDocEditor;
    QSqlQuery qRemoveDocEditor;
    QSqlQuery qRemoveDoc;
    QSqlQuery qCountDocEditors;
    QSqlQuery qSelectUserDocs;
    QSqlQuery qExistUser;

public:

    // Neutral ctor and dtor; the initialization work is done inside open()

    ServerDatabase() {};

    ~ServerDatabase() {};

    // Database connection and initialization
    // the database file is created if it doesn't exist
    void open(QString dbName, Ui::MainWindow* ui);

    // Queries

    void insertUser(const UserData& user);
    void updateUser(const UserData& user);
    void addDocToUser(QString username, QString uri);
    void removeDocFromUser(QString username, QString uri);
    void removeDoc(QString uri);

    int getMaxUserID();
    QList<UserData> readUsersList();
    UserData readUser(QString username);
    QStringList readUserDocuments(QString username);
    QStringList readDocumentURIs();
    int countDocEditors(QString docURI);



};


#endif // SERVERDATABASE_H
