#ifndef STARTUPSECONDSTAKEDDIALOG_H
#define STARTUPSECONDSTAKEDDIALOG_H

#include <QWidget>
#include <homepage.h>
#include <profilepage.h>

namespace Ui {
class startupsecondstakeddialog;
}

class startupsecondstakeddialog : public QWidget
{
    Q_OBJECT

public:
    explicit startupsecondstakeddialog(QWidget *parent = nullptr);
    ~startupsecondstakeddialog();

private:
    Ui::startupsecondstakeddialog *ui;
    HomePage homepage_client{nullptr,nullptr};
    ProfilePage profile_client;

};

#endif // STARTUPSECONDSTAKEDDIALOG_H
