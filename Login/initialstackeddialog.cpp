#include "initialstackeddialog.h"

InitialStackedDialog::InitialStackedDialog(QWidget *parent) :
    QStackedWidget(parent)
{
    QStackedWidget::addWidget(&loginDialog);
    QStackedWidget::addWidget(&registrationDialog);
    QObject::connect(&registrationDialog, &RegistrationDialog::alreadyAnAccountButtonClicked, [this](){QStackedWidget::setCurrentIndex(0);});
    QObject::connect(&loginDialog, &LoginDialog::registrationButtonClicked, [this](){QStackedWidget::setCurrentIndex(1);});
    QStackedWidget::setCurrentIndex(0);
}
