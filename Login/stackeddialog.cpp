#include "stackeddialog.h"

StackedDialog::StackedDialog(QWidget *parent):
    QDialog(parent)
{
    this->stacked.addWidget(&loginDialog);
    this->stacked.addWidget(&registrationDialog);
    QObject::connect(&registrationDialog, &RegistrationDialog::alreadyAnAccountButtonClicked, [this](){this->stacked.setCurrentIndex(0);});
    QObject::connect(&loginDialog, &LoginDialog::registrationButtonClicked, [this](){this->stacked.setCurrentIndex(1);});
    this->stacked.setCurrentIndex(0);
    this->stacked.currentWidget()->show();
}

void StackedDialog::show(){
    QDialog::show();
    this->stacked.currentWidget()->show();
}
