#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include "registrationdialog.h"
#include <QRegularExpression>
#include <QDebug>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT


private:
    Ui::LoginDialog *ui;

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

public slots:
    void onLoginFailure(QString errorMessage);

private slots:
    void on_registration_clicked();
    void on_login_clicked();

signals:
    void loginRequest(QString user, QString password);
    void registrationButtonClicked();
};

#endif // LOGINDIALOG_H
