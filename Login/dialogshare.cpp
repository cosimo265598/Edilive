#include "dialogshare.h"
#include "ui_dialogshare.h"

DialogShare::DialogShare(QWidget *parent, QString URI) :
    QDialog(parent),
    URI(URI),
    ui(new Ui::DialogShare)
{
    ui->setupUi(this);
    ui->URI->setText(URI);
}

DialogShare::~DialogShare()
{
    QDialog::setAttribute(Qt::WA_DeleteOnClose, true);
    delete ui;
}

void DialogShare::on_pushButton_share_clicked()
{
    QString username = ui->username->text();
    QString URI = ui->URI->text();

    if(username.isEmpty() || username.isNull() || URI.isEmpty() || URI.isNull()){
        ui->errorMessage->setStyleSheet("color : red");
        ui->errorMessage->setText("FIll all the fields");
    }
    else{
        emit shareFile(username, URI);
        QDialog::close();
    }
}

void DialogShare::on_pushButton_cancel_clicked()
{
    QDialog::close();
}
