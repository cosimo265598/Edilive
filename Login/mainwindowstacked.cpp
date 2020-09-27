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

    //connections homePage
    connect(this, &MainWindowStacked::receivedFileHandlers, &homePage, &HomePage::onReceivedFileHandlers);
    connect(&homePage, &HomePage::fileHandlerClicked, this, &MainWindowStacked::fileHandlerClicked);
    connect(this, &MainWindowStacked::loadSubscriberInfo, &homePage, &HomePage::onLoadSubscriberInfo);
}

MainWindowStacked::~MainWindowStacked()
{
    delete ui;
}
