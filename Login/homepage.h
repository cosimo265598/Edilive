#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

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
namespace Ui { class HomePage; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    HomePage(QWebSocket *socket = nullptr, QWidget *parent = nullptr);
    ~HomePage();
    void openReceivedFile(QByteArray data);
    void createHomepage(QJsonArray json);
    ProfilePage *getProfilePage();

private Q_SLOTS:
    void onFileHandlerClicked();
    void on_pushButton_new_file_clicked();
    void on_pushButton_aggiorna_vista_clicked();
    void on_pushButton_profile_page_clicked();

private:
    Ui::HomePage *ui;
    QWebSocket *client_socket;
    EventFilterImpl *eventFilter;
    QStringList listfile;
    ProfilePage *profilePage;

};

#endif // HOMEPAGE_H
