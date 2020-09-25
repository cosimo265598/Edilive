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

QT_FORWARD_DECLARE_CLASS(QWebSocket)

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage();
    void openReceivedFile(QByteArray data);
    void createHomepage(QJsonArray json);

private Q_SLOTS:
    void onFileHandlerClicked();
    void on_pushButton_new_file_clicked();
    void on_pushButton_aggiorna_vista_clicked();
    void on_pushButton_profile_page_clicked();

public Q_SLOTS:
     void onLoadFileHandlers(QJsonArray);

private:
    Ui::HomePage *ui;
    QWebSocket *client_socket;
    EventFilterImpl *eventFilter;
    QStringList listfile;

signals:
    void homePageStartup();

};

#endif // HOMEPAGE_H
