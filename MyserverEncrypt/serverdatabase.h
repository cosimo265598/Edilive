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
                     \"FileName\"	TEXT NOT NULL,\
                     FOREIGN KEY(\"Username\") REFERENCES \"Users\"(\"Username\") ON UPDATE CASCADE ON DELETE CASCADE,\
                     PRIMARY KEY(\"Username\", \"FileName\"));"

#define defaultnamedb "sql_for_server"

#define LOG_PRINT_DB  QDateTime::currentDateTime().toString()


class ServerDatabase: public QObject
{
  Q_OBJECT

private:
    Ui::MainWindow *ui_of_server;

    QSqlDatabase db;

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

    ServerDatabase();

    ~ServerDatabase();

    // Database connection and initialization
    // the database file is created if it doesn't exist

    void open(QString dbName, QString connName, Ui::MainWindow* ui);

    // Queries

    void insertUser(const UserData& user);
    void updateUser(const UserData& user);
    void addDocToUser(QString username, QString fileName);
    void removeDocFromUser(QString username, QString fileName);
    void removeDoc(QString fileName);
    void removeDatabase(QString connectionName);
    int getMaxUserID();
    QList<UserData> readUsersList();
    UserData readUser(QString username);
    QStringList readUserDocuments(QString username);
    QStringList readDocuments();
    int countDocEditors(QString fileName);
signals:
     void printUiServerDatabase(const QString&);
};


#endif // SERVERDATABASE_H
