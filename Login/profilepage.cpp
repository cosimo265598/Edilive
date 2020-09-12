#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>
#include <buildermessageclient.h>
#include <QWebSocket>

ProfilePage::ProfilePage(QWidget *parent,QWebSocket* client_socket) : QMainWindow(parent),
    ui(new Ui::ProfilePage),client_socket(client_socket)
{
    ui->setupUi(this);
    // make a query for retrive personal data
    QByteArray out;
    BuilderMessageClient::MessageSendToServer(
                out,
                BuilderMessageClient::MessageOpenProfilePage());
    client_socket->sendBinaryMessage(out);

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


