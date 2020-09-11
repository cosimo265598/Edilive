#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>

ProfilePage::ProfilePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilePage)
{
    ui->setupUi(this);
}

ProfilePage::~ProfilePage()
{
    delete ui;
}


void ProfilePage::on_buttonBox_ok_cancel_accepted()
{
    qDebug()<<"sicuro ?";
}

void ProfilePage::on_buttonBox_ok_cancel_rejected()
{
    qDebug()<<"sicuro ?" <<"rejected";
}

