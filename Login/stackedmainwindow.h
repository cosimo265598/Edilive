#ifndef STACKEDMAINWINDOW_H
#define STACKEDMAINWINDOW_H

#include <QWidget>
#include <homepage.h>
#include <profilepage.h>
#include "ui_stackedmainwindow.h"

namespace Ui {
class StackedMainWindow;
}

class StackedMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StackedMainWindow(QWidget *parent = nullptr);
    ~StackedMainWindow();

private:
    Ui::StackedMainWindow *ui;
    HomePage homePage{nullptr};
    //ProfilePage profile_client;

signals:
    void homePageStartup();
};

#endif // STACKEDMAINWINDOW_H
