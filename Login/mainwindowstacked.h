#ifndef MAINWINDOWSTACKED_H
#define MAINWINDOWSTACKED_H

#include <QWidget>
#include <QMainWindow>
#include <homepage.h>
#include <profilepage.h>
#include <QSizePolicy>

#include "ui_mainwindowstacked.h"

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
    HomePage homePage;
    ProfilePage profilePage;

public slots:

signals:
    void receivedFileHandlers(QJsonArray);
    void fileHandlerDbClicked(QString fileName);
    void loadSubscriberInfo(QString, QString, QByteArray);
    void createNewFileRequest(QString fileName);
    void newFileCreationFailure(QString errorMessage);
    void deleteFileRequest(QString fileName);
    void updateProfileRequest(updateUser_t);
    void updateSuccess();
    void resetSubscriberInfo();
};

#endif // MAINWINDOWSTACKED_H
