#include "registrationdialog.h"
#include "ui_registrationdialog.h"

RegistrationDialog::RegistrationDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationDialog)
{
    ui->setupUi(this);
    RegistrationDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    RegistrationDialog::setFixedSize(371,465);
    RegistrationDialog::setWindowTitle("Registration");
    ui->errorMessage->setStyleSheet("color : red");
}

RegistrationDialog::~RegistrationDialog()
{
    delete ui;
}

void RegistrationDialog::resetPalette(){
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");
    ui->confirmPassword->setStyleSheet("");
}

void RegistrationDialog::on_pushButton_register_clicked()
{
    resetPalette();

    if(ui->username->text().isEmpty() || ui->password->text().isEmpty() || ui->confirmPassword->text().isEmpty()){
        ui->errorMessage->setText("Fill all the fields");
        if(ui->password->text().isEmpty())
            ui->password->setStyleSheet(" border: 1px solid red;");
        if(ui->confirmPassword->text().isEmpty())
            ui->confirmPassword->setStyleSheet(" border: 1px solid red;");
        if(ui->username->text().isEmpty())
            ui->username->setStyleSheet(" border: 1px solid red;");
    }
    if(QString::compare(ui->password->text(), ui->confirmPassword->text(), Qt::CaseInsensitive) != 0){
        ui->password->setStyleSheet(" border: 1px solid red;");
        ui->confirmPassword->setStyleSheet(" border: 1px solid red;");
        ui->errorMessage->setText("The two password MUST be equal");
    }else{
        emit registrationRequest(ui->username->text(), ui->password->text(), ui->confirmPassword->text());
    }
}

void RegistrationDialog::on_pushButton_alreadyAnAccount_clicked()
{
    ui->username->clear();
    ui->password->clear();
    ui->confirmPassword->clear();
    ui->username->setStyleSheet("");
    ui->password->setStyleSheet("");
    ui->confirmPassword->setStyleSheet("");
    ui->errorMessage->clear();

    emit alreadyAnAccount();
}
