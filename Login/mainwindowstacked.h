#ifndef MAINWINDOWSTACKED_H
#define MAINWINDOWSTACKED_H

#include <QWidget>
#include <homepage.h>
#include <profilepage.h>

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
    //ProfilePage profile_client;

public slots:

signals:
    void loadFileHandlers(QJsonArray);
    void fileHandlerClicked(QString fileName);
};

#endif // MAINWINDOWSTACKED_H
