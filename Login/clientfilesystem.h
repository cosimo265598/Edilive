#ifndef CLIENTFILESYSTEM_H
#define CLIENTFILESYSTEM_H

#include <QMainWindow>
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QStandardItem>
#include <QToolButton>
#include <QMessageBox>
#include <QInputDialog>
#include "filehandler.h"
#include "eventfilterimpl.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

QT_BEGIN_NAMESPACE
namespace Ui { class ClientFilesystem; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class ClientFilesystem : public QMainWindow
{
    Q_OBJECT

public:
    ClientFilesystem(QWidget *parent = nullptr,QWebSocket *socket=nullptr);
    ~ClientFilesystem();
    void openReceivedFile(QByteArray data);
    void createHomepage(QJsonArray json);

private Q_SLOTS:
    void onFileHandlerClicked();
    void on_pushButton_new_file_clicked();

    void on_pushButton_aggiorna_vista_clicked();

private:
    Ui::ClientFilesystem *ui;
    QWebSocket *client_socket;
    EventFilterImpl *eventFilter;
    QStringList listfile;
    int c=0;
};
#endif // CLIENTFILESYSTEM_H
