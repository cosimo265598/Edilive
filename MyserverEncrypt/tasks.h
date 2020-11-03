#ifndef TASKS_H
#define TASKS_H

#include <QObject>
#include <QWebSocket>
#include <QThread>
#include <QJsonObject>
#include <QSqlDatabase>

#include "serverdatabase.h"
#include "serverexception.h"
#include "userdata.h"
#include "workspace.h"
#include "client.h"
#include "buildermessage.h"
#include "ui_mainwindow.h"

enum TypeOperation : quint16
{
    // Tests
    Simplemessage,
    // Login messages
    LoginRequest,
    LoginChallenge,
    LoginUnlock,
    LoginGranted,
    LoginError,

    // Account messages
    AccountCreate,
    AccountUpdate,
    AccountConfirmed,
    AccountError,

    // File Management
    OpenDirectory,
    OpenFile,
    CreateFile,
    ErrorFile,
    // Logout message
    Logout,
    // Others
    Failure,

    // personal image
    ProfileData,
    DeleteFile,
    // add other enum below, and do not change the order of list
    InsertionChar,
    DeletionChar,
    RemoveClientFromWorkspace,
    ShareFile

};


class Tasks : public QObject, public QRunnable
{
    Q_OBJECT
public:

    Tasks(QObject *parent,
          QJsonObject request,
          QWebSocket* socket,
          QMap<QWebSocket*, QSharedPointer<Client>>& clients,
          QMap<QString, UserData>& users,
          QMap<QString, QSharedPointer<Workspace>>& workspaces,
          TypeOperation type,
          Ui::MainWindow* ui
    );

    void run() override;
    ~Tasks();

private:
    QMap<QWebSocket*, QSharedPointer<Client>>& clients;
    QMap<QString, UserData>& users;
    QMap<QString, QSharedPointer<Workspace>>& workspaces;
    TypeOperation typeOp;
    QJsonObject request;
    QWebSocket* socket;
    QString threadId;
    Ui::MainWindow *ui;
    QString rootPath = QDir().currentPath()+"/files/";

    void serverAccountCreate(QJsonObject request);
    void serverLoginRequest();
    void serverLoginUnlock();
    void serverAccountCreate();
    void serverOpenDirOfClient(QWebSocket *);
    void serverUpdateProfileClient();
    void serverPersonalDataOfClient();
    void serverCreateFileForClient();
    void serverDeleteFileForClient();
    void serverOpenFile();
    void serverInsertionChar();
    void serverDeletionChar();
    void serverRemoveClientFromWorkspace();
    void serverShareFile();

    //QString generateURI(QString creator, QString fileName) const;

signals:
    void printUiServer(QString);
    void loginError(QWebSocket*,QString);
    void messageChallenge(QWebSocket*, QString, QString);
    void messageChallegePassed(QWebSocket*, QString);
    void accountCreationError(QWebSocket*, QString);
    void accountConfirmed(QWebSocket*, QString);
    void openDirOfClient(QWebSocket*, QJsonArray);
    void updateProfileClient(QWebSocket*, QJsonArray);
    void accountUpdateSuccess(QWebSocket*, QString);
    void accountUpdateError(QWebSocket*, QString);
    void personalDataOfClient(QWebSocket*, QString, QString, QImage);
    void fileCreationError(QWebSocket*, QString);
    void fileDeletionError(QWebSocket*, QString);
    void openFile(QWebSocket*, QString);
    void socketAbort(QWebSocket*);
    void insertionChar(QWebSocket *, Symbol);
    void deletionChar (QWebSocket *, Symbol);
    void removeClientFromWorkspace(QWebSocket*, QString);
    //void requestShareFile(QWebSocket*, QString, QString);
};

#endif // TASKS_H
