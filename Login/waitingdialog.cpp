#include "waitingdialog.h"

QT_USE_NAMESPACE

WaitingDialog::WaitingDialog(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::WaitingDialog)
{
    ui->setupUi(this);
}

WaitingDialog::~WaitingDialog()
{
    delete ui;
}
