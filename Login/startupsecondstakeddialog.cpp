#include "startupsecondstakeddialog.h"
#include "ui_startupsecondstakeddialog.h"

startupsecondstakeddialog::startupsecondstakeddialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::startupsecondstakeddialog)
{
    qDebug()<<"called costructor second dialog";
    ui->setupUi(this);
    startupsecondstakeddialog::setAttribute(Qt::WA_DeleteOnClose, true);
    startupsecondstakeddialog::setFixedSize(Qt::PreferredSize,Qt::PreferredSize);
    ui->stackedWidget->addWidget(&homepage_client);
    ui->stackedWidget->addWidget(&profile_client);
    ui->stackedWidget->setCurrentIndex(0);

}

startupsecondstakeddialog::~startupsecondstakeddialog()
{
    delete ui;
}
