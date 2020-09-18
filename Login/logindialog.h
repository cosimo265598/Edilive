#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>

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
signals:
    void loginRequest(QString user, QString password);
    void sendCredentials(QString user, QString pwd);
};

#endif // LOGINDIALOG_H
