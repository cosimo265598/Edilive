#include "registrationdialog.h"
#include "ui_registrationdialog.h"

RegistrationDialog::RegistrationDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegistrationDialog)
{
    ui->setupUi(this);
<<<<<<< HEAD
    RegistrationDialog::setFixedSize(371,465);
=======
    RegistrationDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    //RegistrationDialog::setFixedSize(371,465);
    RegistrationDialog::setWindowTitle("Registration");
>>>>>>> StackedDialogTest
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

    QString username = ui->username->text().simplified();
    QString password = ui->password->text().simplified();
    QString confirmPassword = ui->confirmPassword->text().simplified();

    QRegularExpression regex("^([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]{2,5})$");
    QRegularExpressionMatch match = regex.match(username);

    if(username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()){
        ui->errorMessage->setText("Fill all the fields");
        if(password.isEmpty())
            ui->password->setStyleSheet(" border: 1px solid red;");
        if(confirmPassword.isEmpty())
            ui->confirmPassword->setStyleSheet(" border: 1px solid red;");
        if(username.isEmpty())
            ui->username->setStyleSheet(" border: 1px solid red;");
    }else if(QString::compare(password,confirmPassword, Qt::CaseInsensitive) != 0){
        ui->password->setStyleSheet(" border: 1px solid red;");
        ui->confirmPassword->setStyleSheet(" border: 1px solid red;");
        ui->errorMessage->setText("The two password MUST be equal");
    }else if(!match.hasMatch()){
        ui->username->setStyleSheet(" border: 1px solid red;");
        ui->errorMessage->setText("Insert a valid email as username");
    }else{
        emit registrationRequest(ui->username->text(), ui->password->text());
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

    emit alreadyAnAccountButtonClicked();
}

void RegistrationDialog::onRegistrationFailure(QString errorMessage){
    ui->errorMessage->setText(errorMessage);
}
