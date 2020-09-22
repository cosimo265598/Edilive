#include "startupstackeddialog.h"


StartupStackedDialog::StartupStackedDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupStackedDialog)
{
    ui->setupUi(this);
    StartupStackedDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    StartupStackedDialog::setFixedSize(371,465);
    ui->stackedWidget->addWidget(&loginDialog);
    ui->stackedWidget->addWidget(&registrationDialog);
    ui->stackedWidget->setCurrentIndex(0);

    QObject::connect(&registrationDialog, &RegistrationDialog::alreadyAnAccountButtonClicked, [this](){ui->stackedWidget->setCurrentIndex(0);});
    QObject::connect(&loginDialog, &LoginDialog::registrationButtonClicked, [this](){ui->stackedWidget->setCurrentIndex(1);});

    QObject::connect(&loginDialog, &LoginDialog::loginRequest, this, &StartupStackedDialog::loginRequest);
    QObject::connect(this, &StartupStackedDialog::loginFailure, &loginDialog, &LoginDialog::on_login_failure);
}

StartupStackedDialog::~StartupStackedDialog()
{
    delete ui;
}

void StartupStackedDialog::onLoginFailure(){
    emit loginFailure();
}
