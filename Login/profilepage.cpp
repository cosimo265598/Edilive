#include "profilepage.h"
#include "ui_profilepage.h"
#include <QDialogButtonBox>
#include <QDebug>
#include "buildermessageclient.h"


ProfilePage::ProfilePage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilePage),
    pixmap(new QPixmap()),
    updateUser(new UpdateUser())
{
    ui->setupUi(this);
}

ProfilePage::~ProfilePage()
{
    delete ui;
    delete pixmap;
    delete updateUser;
}


void ProfilePage::on_buttonBox_ok_cancel_accepted()
{
    if(updateUser->getPassword() == nullptr && updateUser->getNickname() == nullptr && updateUser->getSerializedImage() == nullptr){
        QMessageBox::critical(this, tr("WARNING"),"No changes detected",QMessageBox::Ok);
        return;
    }
    emit updateProfileRequest(*updateUser);
    updateUser->reset();
}

void ProfilePage::on_buttonBox_ok_cancel_rejected()
{
    qDebug()<<"sicuro ?" <<"rejected";
}

/* Lasciamo perdere il discorso della immagine circolare???
 *
 *
void ProfilePage::setImageProfile(QString& path)
{
    QPixmap pixmap(path);
    //QPixmap scaled = pixmap.scaled(ui->profileImage->width(), ui->profileImage->height(), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    //ui->profileImage->setPixmap(scaled);

}
*/

void ProfilePage::on_pushButton_returnToHome_clicked()
{
    emit returnToHomeClicked();
}

void ProfilePage::onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage){
    ui->username->setText(username);
    ui->nickname->setText(nickname);
    //this->pixmap->loadFromData(serializedImage);
    //ui->accountImage->setPixmap(*pixmap);
    loadImage();
}


void ProfilePage::loadImage(){
    QString path = QDir().homePath()+ "/QtProjects/pds-project/myservertest/Login/images/default.png";
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    this->pixmap->loadFromData(file.readAll());
    ui->accountImage->setPixmap(*pixmap);
    file.close();
}


void ProfilePage::on_pushButton_changeImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose an Image (PNG format)"), QDir::homePath(), tr("Image Files (*.png)"));
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray serializedImage = file.readAll();
    file.close();
    this->pixmap->loadFromData(serializedImage);
    ui->accountImage->setPixmap(*pixmap);
    updateUser->setSerializedImage(serializedImage);
}

void ProfilePage::on_nickname_editingFinished()
{
    if(ui->nickname->text()==nullptr)
        updateUser->setNickname(nullptr);
    else
        updateUser->setNickname(ui->nickname->text());
}

void ProfilePage::on_password_editingFinished()
{
    if(ui->password->text() != ui->confirmPassword->text())
       updateUser->setPassword(nullptr);
    else
       updateUser->setPassword(ui->password->text());
}

void ProfilePage::on_confirmPassword_editingFinished()
{
    if(ui->confirmPassword->text() != ui->password->text())
        updateUser->setPassword(nullptr);
    else
        updateUser->setPassword(ui->password->text());
}
