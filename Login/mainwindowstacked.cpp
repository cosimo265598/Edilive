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
    ui->stackedWidget->addWidget(&homePage);
    //ui->stackedWidget->addWidget(&profile_client);
    ui->stackedWidget->setCurrentIndex(0);

    connect(this, &MainWindowStacked::loadFileHandlers, &homePage, &HomePage::onLoadFileHandlers);
}

MainWindowStacked::~MainWindowStacked()
{
    delete ui;
}
