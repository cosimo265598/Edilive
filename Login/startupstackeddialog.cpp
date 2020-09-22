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

    //Internal loginDialog and registrationDialog
    QObject::connect(&registrationDialog, &RegistrationDialog::alreadyAnAccountButtonClicked, [this](){ui->stackedWidget->setCurrentIndex(0);});
    QObject::connect(&loginDialog, &LoginDialog::registrationButtonClicked, [this](){ui->stackedWidget->setCurrentIndex(1);});

    //loginDialog connections
    QObject::connect(&loginDialog, &LoginDialog::loginRequest, this, &StartupStackedDialog::loginRequest);
    QObject::connect(this, &StartupStackedDialog::loginFailure, &loginDialog, &LoginDialog::onLoginFailure);

    //registrationDialog connections
    QObject::connect(&registrationDialog, &RegistrationDialog::registrationRequest, this, &StartupStackedDialog::registrationRequest);
    QObject::connect(this, &StartupStackedDialog::loginFailure, &loginDialog, &LoginDialog::onLoginFailure);
}

StartupStackedDialog::~StartupStackedDialog()
{
    delete ui;
}

void StartupStackedDialog::onLoginFailure(){
    emit loginFailure();
}
