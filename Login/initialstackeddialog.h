#ifndef INITIALSTACKEDDIALOG_H
#define INITIALSTACKEDDIALOG_H

#include <QObject>
#include <QStackedWidget>

#include "logindialog.h"
#include "registrationdialog.h"

class InitialStackedDialog : public QStackedWidget
{
    Q_OBJECT
public:
    explicit InitialStackedDialog(QWidget *parent = nullptr);

private:
    LoginDialog loginDialog;
    RegistrationDialog registrationDialog;
};

#endif // INITIALSTACKEDDIALOG_H
