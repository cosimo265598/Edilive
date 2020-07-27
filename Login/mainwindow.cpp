#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fieldForSign(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::fieldForSign(bool state){
    if(!state){
        ui->sign->show();
        ui->login->show();

        ui->labelconfpsw->hide();
        ui->confpsw->hide();
        ui->sign_account->hide();
    }else
    {
        ui->labelconfpsw->show();
        ui->confpsw->show();
        ui->sign_account->show();

        ui->sign->hide();
        ui->login->hide();
    }
}


void MainWindow::on_sign_clicked()
{
    fieldForSign(true);
}

void MainWindow::on_back_clicked()
{
    fieldForSign(false);
}

void MainWindow::on_login_clicked()
{
    m_webSocket.open(QStringLiteral("wss://localhost:1234"));
    // new connection try to login

}
