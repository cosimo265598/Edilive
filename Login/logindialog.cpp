#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //LoginDialog::setFixedSize(371,465);
    ui->errorMessage->setStyleSheet("color : red");
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::onLoginFailure(QString errorMessage){
    ui->username->clear();
    ui->password->clear();
    ui->username->setStyleSheet(" border: 1px solid red;");
    ui->password->setStyleSheet(" border: 1px solid red;");
    ui->errorMessage->setText(errorMessage);
}

void LoginDialog::on_registration_clicked()
{
    ui->username->clear();
    ui->password->clear();
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");
    ui->errorMessage->clear();
    emit registrationButtonClicked();
}

void LoginDialog::on_login_clicked()
{
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");

    QString username = ui->username->text().simplified();
    QString password = ui->password->text().simplified();

    QRegularExpression regex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpressionMatch match = regex.match(username);

    if(username.isEmpty() || password.isEmpty()){
        ui->errorMessage->setText("Fill all the fields");
        if(password.isEmpty())
            ui->password->setStyleSheet(" border: 1px solid red;");
        if(password.isEmpty())
            ui->username->setStyleSheet(" border: 1px solid red;");
    }else if(!match.hasMatch() && username!="ciao"){ //TODO: remove backdoor "ciao"
        ui->username->setStyleSheet(" border: 1px solid red;");
        ui->errorMessage->setText("Insert a valid email as username");
    }else
        emit loginRequest(ui->username->text(), ui->password->text());
}
