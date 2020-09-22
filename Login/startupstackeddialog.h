#ifndef STARTUPSTACKEDDIALOG_H
#define STARTUPSTACKEDDIALOG_H

#include <QWidget>

#include "ui_startupstackeddialog.h"
#include "logindialog.h"
#include "registrationdialog.h"

namespace Ui {
class StartupStackedDialog;
}

class StartupStackedDialog : public QWidget
{
    Q_OBJECT

public:
    explicit StartupStackedDialog(QWidget *parent = nullptr);
    ~StartupStackedDialog();

private:
    Ui::StartupStackedDialog *ui;
    LoginDialog loginDialog;
    RegistrationDialog registrationDialog;

public slots:
    void onLoginFailure();

signals:
    void loginRequest(QString user, QString password);
    void loginFailure();
    void registrationRequest(QString user, QString password);

};
#endif // STARTUPSTACKEDDIALOG_H
