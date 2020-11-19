#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>
#include "buildermessageclient.h"


ProfilePage::ProfilePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilePage)
{
    ui->setupUi(this);
    resetUpdateUser();
    resetFields();
}

ProfilePage::~ProfilePage()
{
    delete ui;
}


void ProfilePage::on_buttonBox_ok_cancel_accepted()
{
    if(this->managePasswordChange()){
        if(QString::compare(ui->password->text().simplified(), ui->confirmPassword->text().simplified(), Qt::CaseInsensitive) != 0){
            updateUser.password.clear();
            ui->password->setStyleSheet(" border: 1px solid red;");
            ui->confirmPassword->setStyleSheet(" border: 1px solid red;");
            QMessageBox::critical(this, tr("WARNING"),"The two password MUST be equal",QMessageBox::Ok);
            return;
        }else{
            updateUser.password = ui->password->text().simplified();
        }
    }

    if(ui->nickname->text().simplified().isNull() || ui->nickname->text().simplified().isEmpty() || ui->nickname->text().simplified() == nickname_old)
        updateUser.nickname.clear();
    else
        updateUser.nickname =  ui->nickname->text().simplified();

    if(updateUser.nickname == nullptr && updateUser.password == nullptr && updateUser.serializedImage == nullptr){
        QMessageBox::critical(this, tr("WARNING"),"No changes detected",QMessageBox::Ok);
        return;
    }
    if(QMessageBox::critical(this, tr("WARNING"),"Apply the changes?",QMessageBox::Ok,QMessageBox::Cancel) == 0x00000400){
        emit updateProfileRequest(updateUser);
        resetFields();
        resetUpdateUser();
    }else
        return;
}

void ProfilePage::on_buttonBox_ok_cancel_rejected()
{
    if(updateUser.nickname != nullptr || updateUser.password != nullptr || updateUser.serializedImage != nullptr){
        if(QMessageBox::critical(this, tr("WARNING"),"Delete changes? The operation will not reversible",QMessageBox::Ok, QMessageBox::Cancel ) == 0x00000400){
            resetUpdateUser();
            resetFields();
            emit resetSubscriberInfo();
        }
    }
}

void ProfilePage::on_pushButton_returnToHome_clicked()
{
    if(updateUser.nickname != nullptr || updateUser.password != nullptr || updateUser.serializedImage != nullptr){
        if(QMessageBox::critical(this, tr("WARNING"),"Delete changes? The operation will not reversible",QMessageBox::Ok, QMessageBox::Cancel ) == 0x00000400){
            resetUpdateUser();
            resetFields();
            emit resetSubscriberInfo();
            emit returnToHomeClicked();
        }
    }
    else{
        resetFields();
        emit returnToHomeClicked();
    }
}

void ProfilePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    nickname_old = nickname;
    ui->username->setText(username);
    ui->nickname->setText(nickname);
    loadImage(serializedImage);
}

void ProfilePage:: onAccountUpdateError(QString message)
{
    resetUpdateUser();
    resetFields();
    QMessageBox::critical(this, tr("WARNING"),message,QMessageBox::Ok);
    return;
}

void ProfilePage::onReloadProfilePageInfo(QString username, QString nickname, QByteArray serializedImage)
{
    ui->username->setText(username);
    ui->nickname->setText(nickname);
    loadImage(serializedImage);
}

void ProfilePage::loadImage(QByteArray serializedImage){
    QPixmap pixmap;
    if (serializedImage == nullptr){
        pixmap.load(":/icons_pack/avatar_default.png");
    }else{
        pixmap.loadFromData(serializedImage);
    }

    ui->accountImage->setPixmap( pixmap.scaled(180 ,180, Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


void ProfilePage::on_pushButton_changeImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose an Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.jpeg)"));
    if(path!=nullptr){
        QFile file(path);
        QPixmap pixmap;
        file.open(QIODevice::ReadOnly);
        QByteArray serializedImage = file.readAll();
        file.close();
        pixmap.loadFromData(serializedImage);
        ui->accountImage->setPixmap( pixmap.scaled(ui->accountImage->width(), ui->accountImage->height(), Qt::KeepAspectRatio,Qt::SmoothTransformation));
        updateUser.serializedImage = serializedImage;
    }
}

/*
void ProfilePage::on_nickname_editingFinished()
{
    if(ui->nickname->text().simplified().isNull() || ui->nickname->text().simplified().isEmpty())
        updateUser.nickname.clear();
    else
        updateUser.nickname =  ui->nickname->text().simplified();
}
*/

void ProfilePage::resetUpdateUser(){
    updateUser.username.clear();
    updateUser.nickname.clear();
    updateUser.password.clear();
    updateUser.serializedImage = nullptr;
}

void ProfilePage::resetFields(){

    ui->password->setStyleSheet("");
    ui->confirmPassword->setStyleSheet("");
    ui->password->setText(nullptr);
    ui->confirmPassword->setText(nullptr);
}

bool ProfilePage::managePasswordChange(){

    bool check = false;

    if(!ui->password->text().simplified().isNull() && !ui->password->text().simplified().isEmpty()){
        check = true;
   }

    if(!ui->confirmPassword->text().simplified().isNull() && !ui->confirmPassword->text().simplified().isEmpty()){
        check = true;
    }

   return check;
}
