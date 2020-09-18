#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include "registrationdialog.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

public slots:
    void on_login_clicked();

private:
    Ui::LoginDialog *ui;
    RegistrationDialog regDialog;

signals:
    void loginRequest(QString user, QString password);
    void sendCredentials(QString user, QString pwd);
private slots:
    void on_registration_clicked();
};

#endif // LOGINDIALOG_H
