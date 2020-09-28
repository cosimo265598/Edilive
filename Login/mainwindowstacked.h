#ifndef MAINWINDOWSTACKED_H
#define MAINWINDOWSTACKED_H

#include <QWidget>
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
    HomePage homePage{nullptr};
    ProfilePage profile_client;

public slots:

signals:
    void receivedFileHandlers(QJsonArray);
    void fileHandlerDbClicked(QString fileName);
    void loadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage);
    void createNewFileRequest(QString fileName);
    void newFileCreationFailure(QString errorMessage);
};

#endif // MAINWINDOWSTACKED_H