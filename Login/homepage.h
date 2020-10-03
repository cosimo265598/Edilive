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
#include <QtCore/QDebug>
#include <QCoreApplication>
#include <QDir>

#include "ui_homepage.h"
#include "buildermessageclient.h"
#include "filehandler.h"
#include "eventfilterimpl.h"
#include "subscriber.h"

QT_FORWARD_DECLARE_CLASS(QWebSocket)

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class HomePage : public QWidget
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage();

private Q_SLOTS:
    void onFileHandlerDbClicked();
    void onFileHandlerClicked();
    void on_pushButton_new_file_clicked();
    void on_pushButton_aggiorna_vista_clicked();
    void on_pushButton_profile_page_clicked();
    void on_pushButton_Logout_clicked();
    void onFocusChange(QWidget *old, QWidget *now);

public Q_SLOTS:
     void onReceivedFileHandlers(QJsonArray);
     void onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedimage);
     void onNewFileCreationFailure(QString errorMessage);

private:
    Ui::HomePage *ui;

    // row e column da togliere se non ottimizziamo per non ricaricare la pagina ad ogni inserzione, cancellazione
    int row;
    int column;

    EventFilterImpl *eventFilter;
    QStringList listfile;
    QPixmap *pixmap;
    FileHandler *selected;

    void deleteFile();
    void loadImage();

signals:
    void fileHandlerDbClicked(QString fileName);
    void updateAccountClicked();
    void createNewFileRequest(QString fileName);
    void deleteFileRequest(QString fileName);
};

#endif // HOMEPAGE_H
