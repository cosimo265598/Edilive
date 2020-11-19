#include "mainwindowstacked.h"
#include "ui_mainwindowstacked.h"
#include <QSizePolicy>

MainWindowStacked::MainWindowStacked(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindowStacked)
{
    ui->setupUi(this);

    //qDebug()<<"called costructor second dialog";

    //this->setFixedSize(1200, 800);

    MainWindowStacked::setAttribute(Qt::WA_DeleteOnClose, true);
    ui->stackedWidget->addWidget(&homePage);
    ui->stackedWidget->addWidget(&profilePage);
    ui->stackedWidget->setCurrentIndex(0);


    //connections homePage
    connect(this, &MainWindowStacked::receivedFileHandlers, &homePage, &HomePage::onReceivedFileHandlers);
    connect(&homePage, &HomePage::fileHandlerDbClicked, this, &MainWindowStacked::fileHandlerDbClicked);
    connect(this, &MainWindowStacked::loadSubscriberInfo, &homePage, &HomePage::onLoadSubscriberInfo);
    connect(&homePage, &HomePage::updateAccountClicked,  [this](){MainWindowStacked::setWindowTitle("Update account"); ui->stackedWidget->setCurrentIndex(1);});
    connect(&homePage, &HomePage::createNewFileRequest, this, &MainWindowStacked::createNewFileRequest);
    connect(this, &MainWindowStacked::newFileCreationFailure, &homePage, &HomePage::onNewFileCreationFailure);
    connect(&homePage, &HomePage::deleteFileRequest, this, &MainWindowStacked::deleteFileRequest);
    connect(&homePage, SIGNAL(shareFile(QString, QString)), this, SIGNAL(shareFile(QString, QString)));

    //connection ProfilePage
    connect(&profilePage, &ProfilePage::returnToHomeClicked,  [this](){MainWindowStacked::setWindowTitle("HomePage"); ui->stackedWidget->setCurrentIndex(0);});
    connect(this, &MainWindowStacked::loadSubscriberInfo, &profilePage, &ProfilePage::onLoadSubscriberInfo);
    connect(this, &MainWindowStacked::reloadProfilePageInfo, &profilePage, &ProfilePage::onReloadProfilePageInfo);
    connect(&profilePage, &ProfilePage::updateProfileRequest, this, &MainWindowStacked::updateProfileRequest);
    connect(&profilePage, &ProfilePage::resetSubscriberInfo, this, &MainWindowStacked::resetSubscriberInfo);
    connect(this, &MainWindowStacked::accountUpdateError, &profilePage, &ProfilePage::onAccountUpdateError);
}

MainWindowStacked::~MainWindowStacked()
{
    delete ui;
}
