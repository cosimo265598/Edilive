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
    connect(&homePage, &HomePage::fileHandlerDbClicked, this, &MainWindowStacked::fileHandlerDbClicked);
    connect(this, &MainWindowStacked::loadSubscriberInfo, &homePage, &HomePage::onLoadSubscriberInfo);
    connect(&homePage, &HomePage::updateAccountClicked,  [this](){MainWindowStacked::setWindowTitle("Update account"); ui->stackedWidget->setCurrentIndex(1);});
    connect(&homePage, &HomePage::createNewFileRequest, this, &MainWindowStacked::createNewFileRequest);
    connect(this, &MainWindowStacked::newFileCreationFailure, &homePage, &HomePage::onNewFileCreationFailure);
    connect(&homePage, &HomePage::deleteFileRequest, this, &MainWindowStacked::deleteFileRequest);

    //connection ProfilePage
    //connect(&profilePage, &HomePage::updateAccountClicked,  [this](){MainWindowStacked::setWindowTitle("Update account"); ui->stackedWidget->setCurrentIndex(1);});
    //connect(this, &MainWindowStacked::loadSubscriberInfo, &profilePage, &ProfilePage::onLoadSubscriberInfo);

}

MainWindowStacked::~MainWindowStacked()
{
    delete ui;
}
