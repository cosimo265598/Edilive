#include "startupstackeddialog.h"


StartupStackedDialog::StartupStackedDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupStackedDialog)
{
    ui->setupUi(this);
    StartupStackedDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    //StartupStackedDialog::setFixedSize(371,465);
    ui->stackedWidget->addWidget(&loginDialog);
    ui->stackedWidget->addWidget(&registrationDialog);
    ui->stackedWidget->setCurrentIndex(0);
    StartupStackedDialog::setWindowTitle("Login");

    //Internal loginDialog and registrationDialog
    QObject::connect(&registrationDialog, &RegistrationDialog::alreadyAnAccountButtonClicked, [this](){StartupStackedDialog::setWindowTitle("Login"); ui->stackedWidget->setCurrentIndex(0);});
    QObject::connect(&loginDialog, &LoginDialog::registrationButtonClicked, [this](){StartupStackedDialog::setWindowTitle("Registration"); ui->stackedWidget->setCurrentIndex(1);});

    //loginDialog connections
    QObject::connect(&loginDialog, &LoginDialog::loginRequest, this, &StartupStackedDialog::loginRequest);
    QObject::connect(this, &StartupStackedDialog::loginFailure, &loginDialog, &LoginDialog::onLoginFailure);

    //registrationDialog connections
    QObject::connect(&registrationDialog, &RegistrationDialog::registrationRequest, this, &StartupStackedDialog::registrationRequest);
    QObject::connect(this, &StartupStackedDialog::registrationFailure, &registrationDialog, &RegistrationDialog::onRegistrationFailure);
}

StartupStackedDialog::~StartupStackedDialog()
{
    delete ui;
}

void StartupStackedDialog::onLoginFailure(QString errorMessage){
    emit loginFailure(errorMessage);
}

void StartupStackedDialog::onRegistrationFailure(QString errorMessage){
    emit registrationFailure(errorMessage);
}
