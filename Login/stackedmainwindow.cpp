#include "stackedmainwindow.h"

StackedMainWindow::StackedMainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StackedMainWindow)
{
    qDebug()<<"called costructor second dialog";
    ui->setupUi(this);
    StackedMainWindow::setAttribute(Qt::WA_DeleteOnClose, true);
    StackedMainWindow::setFixedSize(Qt::PreferredSize,Qt::PreferredSize);
    ui->stackedWidget->addWidget(&homePage);
    //ui->stackedWidget->addWidget(&profile_client);
    ui->stackedWidget->setCurrentIndex(0);

    //connections homePage
    QObject::connect(&homePage, &HomePage::homePageStartup, this, &StackedMainWindow::homePageStartup);

}
StackedMainWindow::~StackedMainWindow()
{
    delete ui;
}
