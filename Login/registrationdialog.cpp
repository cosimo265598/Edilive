#include "registrationdialog.h"
#include "ui_registrationdialog.h"

RegistrationDialog::RegistrationDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationDialog)
{
    ui->setupUi(this);
    RegistrationDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    RegistrationDialog::setFixedSize(371,465);
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}
