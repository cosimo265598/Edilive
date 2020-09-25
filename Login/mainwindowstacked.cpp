#include "mainwindowstacked.h"
#include "ui_mainwindowstacked.h"

MainWindowStacked::MainWindowStacked(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindowStacked)
{
    ui->setupUi(this);

    qDebug()<<"called costructor second dialog";
    ui->setupUi(this);
    MainWindowStacked::setAttribute(Qt::WA_DeleteOnClose, true);
    MainWindowStacked::setFixedSize(Qt::PreferredSize,Qt::PreferredSize);
    ui->stackedWidget->addWidget(&homePage);
    //ui->stackedWidget->addWidget(&profile_client);
    ui->stackedWidget->setCurrentIndex(0);

    //connections homePage
    QObject::connect(&homePage, &HomePage::homePageStartup, this, &MainWindowStacked::homePageStartup);
}

MainWindowStacked::~MainWindowStacked()
{
    delete ui;
}
