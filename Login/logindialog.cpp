#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    LoginDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    LoginDialog::setFixedSize(371,465);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_clicked()
{
    emit loginRequest(ui->user->text(), ui->password->text());
}
