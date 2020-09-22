#include "startupstackeddialog.h"
#include "ui_startupstackeddialog.h"

StartupStackedDialog::StartupStackedDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartupStackedDialog)
{
    ui->setupUi(this);
}

StartupStackedDialog::~StartupStackedDialog()
{
    delete ui;
}
