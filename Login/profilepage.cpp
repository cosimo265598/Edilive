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

ProfilePage::ProfilePage(QWidget *parent,QWebSocket* client_socket) :
    QMainWindow(parent)
  , ui(new Ui::ProfilePage)
  , client_socket(client_socket)
{
    ui->setupUi(this);
    mod=setModify(false);
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
    QString path=QDir().tempPath()+"/avatar.png";
    bool stateicon=j.value("ico_present").toBool();
    if(stateicon){
        // l'immagine del profilo non è quella di default
        auto data=j.value("ico").toString().toLatin1();
        QPixmap p;
        p.loadFromData(QByteArray::fromBase64(data), "PNG");

        if(!p.save(path)){
            qDebug()<<"not saved for error";
            return;
        }

        setImageProfile(path);
    } else {
        // usare l'immagine di default
        QString pathdefault=":/icons_pack/avatar_default.png";
        setImageProfile(pathdefault);

    }
    ui->lineEdit_nick->setText(j.value("nickname").toString());
    ui->username_value->setText(j.value("username").toString());

}

void ProfilePage::setImageProfile(QString& path)
{
    QPixmap pixmap(path);
    QPixmap scaled = pixmap.scaled(ui->profileImage->width(), ui->profileImage->height(), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->profileImage->setPixmap(scaled);

}

bool ProfilePage::setModify(bool value)
{
    ui->lineEdit_nick->setEnabled(value);
    ui->lineEdit_password->setEnabled(value);
    ui->lineEdit_confirm_password->setEnabled(value);
    return value;
}



void ProfilePage::on_pushButton_abilitaModifiche_clicked()
{
    if(mod){
        mod=setModify(false);
        ui->pushButton_abilitaModifiche->setText("Abilita Modifiche");

    }else{
        mod=setModify(true);
        ui->pushButton_abilitaModifiche->setText("Disabilita Modifiche");
    }
}
