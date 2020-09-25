#include "connectionwaitingdialog.h"
#include "ui_connectionwaitingdialog.h"

ConnectionWaitingDialog::ConnectionWaitingDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectionWaitingDialog)
{
    ui->setupUi(this);
}

ConnectionWaitingDialog::~ConnectionWaitingDialog()
{
    delete ui;
}
