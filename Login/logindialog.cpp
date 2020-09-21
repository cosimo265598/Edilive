#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    LoginDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    LoginDialog::setFixedSize(371,465);
    ui->loginFailureMessage->setStyleSheet("color : red");
    ui->stackedWidget->addWidget(&regDialog);
    QObject::connect(&regDialog, &RegistrationDialog::alreadyAnAccount, [this](){ui->stackedWidget->setCurrentIndex(0);});
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_clicked()
{
    emit loginRequest(ui->user->text(), ui->password->text());
}

void LoginDialog::on_login_failure(){
    ui->loginFailureMessage->setText("ERROR, Username or password wrong");
}

void LoginDialog::on_registration_clicked()
{
    ui->loginFailureMessage->clear();
    ui->stackedWidget->setCurrentIndex(1);
}
