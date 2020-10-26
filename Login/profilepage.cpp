#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>
#include "buildermessageclient.h"


ProfilePage::ProfilePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilePage)
{
    resetUpdateUser();
    ui->setupUi(this);
}

ProfilePage::~ProfilePage()
{
    delete ui;
}


void ProfilePage::on_buttonBox_ok_cancel_accepted()
{
    if(updateUser.nickname == nullptr && updateUser.password == nullptr && updateUser.serializedImage == nullptr){
        QMessageBox::critical(this, tr("WARNING"),"No changes detected",QMessageBox::Ok);
        return;
    }
    if(QMessageBox::critical(this, tr("WARNING"),"Apply the changes?",QMessageBox::Ok,QMessageBox::Cancel) == 0x00000400){
        emit updateProfileRequest(updateUser);
        resetUpdateUser();
    }else
        return;
}

void ProfilePage::on_buttonBox_ok_cancel_rejected()
{
    if(updateUser.nickname != nullptr || updateUser.password != nullptr || updateUser.serializedImage != nullptr){
        if(QMessageBox::critical(this, tr("WARNING"),"Delete changes? The operation will not reversible",QMessageBox::Ok, QMessageBox::Cancel ) == 0x00000400){
            resetUpdateUser();
            emit resetSubscriberInfo();
        }
    }
}

void ProfilePage::on_pushButton_returnToHome_clicked()
{
    emit returnToHomeClicked();
}

void ProfilePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    ui->username->setText(username);
    ui->nickname->setText(nickname);
    loadImage(serializedImage);
}


void ProfilePage::loadImage(QByteArray serializedImage){
    QPixmap pixmap;
    if (serializedImage == nullptr){
        pixmap.load(QDir().homePath()+ "/GIT_Saverio/myservertest/Login/images/default.png");
    }else{
        pixmap.loadFromData(serializedImage,"PNG"); //Check if PNG, what happens if not?
    }

    ui->accountImage->setPixmap( pixmap.scaled(ui->accountImage->width(), ui->accountImage->height(), Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


void ProfilePage::on_pushButton_changeImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose an Image (PNG format)"), QDir::homePath(), tr("Image Files (*.png)"));
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

void ProfilePage::on_nickname_editingFinished()
{
    if(ui->nickname->text()==nullptr)
        updateUser.nickname.clear();
    else
        updateUser.nickname =  ui->nickname->text();
}

void ProfilePage::on_password_editingFinished()
{
    if(ui->password->text() != ui->confirmPassword->text())
       updateUser.password.clear();
    else
       updateUser.password = ui->password->text();
}

void ProfilePage::on_confirmPassword_editingFinished()
{
    if(ui->password->text() != ui->confirmPassword->text())
       updateUser.password.clear();
    else
       updateUser.password = ui->password->text();
}

void ProfilePage::resetUpdateUser(){
    updateUser.username.clear();
    updateUser.nickname.clear();
    updateUser.password.clear();
    updateUser.serializedImage.clear();
}
