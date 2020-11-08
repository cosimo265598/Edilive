#ifndef SERVERDATABASE_H
#define SERVERDATABASE_H

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "userdata.h"
#include <QDateTime>
#include <QImage>

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
                     \"URI\"	TEXT NOT NULL,\
                     \"Pending\"	TEXT NOT NULL,\
                     FOREIGN KEY(\"Username\") REFERENCES \"Users\"(\"Username\") ON UPDATE CASCADE ON DELETE CASCADE,\
                     FOREIGN KEY(\"URI\") REFERENCES \"Files\"(\"URI\") ON UPDATE CASCADE ON DELETE CASCADE,\
                     PRIMARY KEY(\"Username\", \"URI\"));"

#define docusertable "CREATE TABLE \"Files\" (\
   \"URI\"	TEXT NOT NULL,\
   \"FileName\"	TEXT NOT NULL,\
   \"Creator\"	TEXT NOT NULL,\
   \"Created\"	TEXT NOT NULL,\
   PRIMARY KEY(\"URI\"));"

#define defaultnamedb "sql_for_server"

#define LOG_PRINT_DB  QDateTime::currentDateTime().toString()

class ServerDatabase: public QObject
{
  Q_OBJECT

private:
    Ui::MainWindow *ui_of_server;

    QSqlDatabase db;
public:

    // Neutral ctor and dtor; the initialization work is done inside open()

    ServerDatabase();

    ~ServerDatabase();

    // Database connection and initialization
    // the database file is created if it doesn't exist

    void open(QString dbName, QString connName, Ui::MainWindow* ui);

    // Queries

    void insertNewDoc(file_t );
    void insertUser(UserData& user);
    void updateUser(const UserData& user);
    bool addDocToUser(QString username, QString URI);
    bool removeDocFromUser(QString username, QString fileName);
    void removeDoc(QString fileName);
    void removeDatabase(QString connectionName);
    int getMaxUserID();
    QList<UserData> readUsersList();
    UserData readUser(QString username);
    QStringList readUserDocuments(QString username);
    QStringList readDocuments();
    int countDocEditors(QString fileName);
    QList<file_t> getUserDocs(QString username);

signals:
     void printUiServerDatabase(const QString&);
};


#endif // SERVERDATABASE_H
