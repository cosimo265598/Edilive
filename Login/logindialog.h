#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include "registrationdialog.h"
#include <QRegularExpression>
#include <QValidator>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT


private:
    Ui::LoginDialog *ui;
    RegistrationDialog regDialog;

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

public slots:
    void on_login_clicked();
    void on_login_failure();

private slots:
    void on_registration_clicked();

signals:
    void loginRequest(QString user, QString password);
    void sendCredentials(QString user, QString pwd);

};

#endif // LOGINDIALOG_H
