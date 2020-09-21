#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    LoginDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    LoginDialog::setFixedSize(371,465);
    LoginDialog::setWindowTitle("Login");
    ui->errorMessage->setStyleSheet("color : red");
    ui->stackedWidget->addWidget(&regDialog);
    QObject::connect(&regDialog, &RegistrationDialog::alreadyAnAccount, [this](){ui->stackedWidget->setCurrentIndex(0);});
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_clicked()
{
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");

    if(!ui->username->text().isEmpty() && !ui->password->text().isEmpty()){
        emit loginRequest(ui->username->text(), ui->password->text());
    }else{
        ui->errorMessage->setText("Fill all the fields");
        if(ui->password->text().isEmpty())
            ui->password->setStyleSheet(" border: 1px solid red;");
        if(ui->username->text().isEmpty())
            ui->username->setStyleSheet(" border: 1px solid red;");
    }
}

void LoginDialog::on_login_failure(){
    ui->username->clear();
    ui->password->clear();
    ui->username->setStyleSheet(" border: 1px solid red;");
    ui->password->setStyleSheet(" border: 1px solid red;");
    ui->errorMessage->setText("ERROR, Username or password wrong");
}

void LoginDialog::on_registration_clicked()
{
    ui->username->clear();
    ui->password->clear();
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");
    ui->errorMessage->clear();
    ui->stackedWidget->setCurrentIndex(1);
}
