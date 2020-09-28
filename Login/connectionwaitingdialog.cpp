#include "connectionwaitingdialog.h"
#include "ui_connectionwaitingdialog.h"

ConnectionWaitingDialog::ConnectionWaitingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWaitingDialog)
{
    ui->setupUi(this);
    ConnectionWaitingDialog::setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
}

ConnectionWaitingDialog::~ConnectionWaitingDialog()
{
    delete ui;
}

void ConnectionWaitingDialog::setText(QString message){
    ui->message->setText(message);
}
