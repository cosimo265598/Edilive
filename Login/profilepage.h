#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QPainter>
#include <profileimage.h>
#include <QWebSocket>
#include <QPixmap>
#include <QBitmap>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "updateuser.h"

namespace Ui {
class ProfilePage;
}

class ProfilePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);
    ~ProfilePage();
    //void setImageProfile(QString& path);

private slots:
    void on_buttonBox_ok_cancel_accepted();
    void on_buttonBox_ok_cancel_rejected();
    void on_pushButton_returnToHome_clicked();
    void on_pushButton_changeImage_clicked();

    void on_nickname_editingFinished();

    void on_password_editingFinished();

    void on_confirmPassword_editingFinished();

public slots:
    void onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage);

private:
    Ui::ProfilePage *ui;
    UpdateUser *updateUser;

    void loadImage();

signals:
    void returnToHomeClicked();
    void updateProfileRequest(UpdateUser updateUser);
};

#endif // PROFILEPAGE_H
