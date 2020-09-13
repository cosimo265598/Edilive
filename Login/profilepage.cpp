#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>
#include "buildermessageclient.h"
#include <QWebSocket>
#include <QPixmap>
#include <QBitmap>
#include <QFile>
#include <QDir>

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

void ProfilePage::viewDataOfClient(QJsonObject j)
{

    // METODO DA SITEMARE PER VISUALLIZAARE UNA NUOVA IMMAGINE

    qDebug()<< "PROVANDO A SETTARE I VALORI";
    bool stateicon=j.value("ico_present").toBool();
    if(stateicon){
        //sostiruire l'immagine corrente
    } else {
        // usare l'immagine di default
    }
    QFile f(":/icons_pack/avatar.png");
    f.rename("avatar_old.png");

    auto data=j.value("ico").toString().toLatin1();
    QPixmap p;
    p.loadFromData(QByteArray::fromBase64(data), "PNG");

    if(p.save(QDir().homePath()+"/GIT/myservertest/Login/icons_pack/new_avatar_default.png"))
        qDebug()<<"saved";

    ui->label_rv->setPixmap(p);

    ui->profileImage->setPixmap(p);

    ui->lineEdit_nick->setText(j.value("nickname").toString());
    ui->username_value->setText(j.value("username").toString());

}


