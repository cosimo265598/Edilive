#ifndef MAINWINDOWSTACKED_H
#define MAINWINDOWSTACKED_H

#include <QWidget>
#include <QMainWindow>
#include <homepage.h>
#include <profilepage.h>

#include "subscriber.h"

namespace Ui {
class MainWindowStacked;
}

class MainWindowStacked : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindowStacked(QWidget *parent = nullptr);
    ~MainWindowStacked();

private:
    Ui::MainWindowStacked *ui;
    HomePage homePage{this};
    ProfilePage profilePage{this};

public slots:

signals:
    void receivedFileHandlers(QJsonArray, QString);
    void fileHandlerDbClicked(QString URI, QString fileName);
    void loadSubscriberInfo(QString, QString, QByteArray);
    void createNewFileRequest(QString fileName);
    void newFileCreationFailure(QString errorMessage);
    void deleteFileRequest(QString URI, QString fileName);
    void updateProfileRequest(updateUser_t);
    void updateSuccess();
    void resetSubscriberInfo();
    void reloadProfilePageInfo(QString username, QString nickname, QByteArray serializedImage);
    void accountUpdateError(QString);
    void shareFile(QString username, QString URI);
};

#endif // MAINWINDOWSTACKED_H
