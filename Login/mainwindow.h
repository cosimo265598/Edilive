#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "profilepage.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr,QWebSocket *socket=nullptr);
    ~MainWindow();
    void openReceivedFile(QByteArray data);
    void createHomepage(QJsonArray json);
    ProfilePage* getProfilePage();

private Q_SLOTS:
    void onFileHandlerClicked();
    void on_pushButton_new_file_clicked();

    void on_pushButton_aggiorna_vista_clicked();

    void on_toolButton_profile_page_clicked();

private:
    Ui::MainWindow *ui;
    QWebSocket *client_socket;
    EventFilterImpl *eventFilter;
    QStringList listfile;
    ProfilePage *pageofclient;
    int c=0;

};
#endif // MAINWINDOW_H
