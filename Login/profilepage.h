#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QPainter>
#include <QWebSocket>
#include <QPixmap>
#include <QBitmap>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "updateuser.h"
#include "subscriber.h"

struct updateUser_t{
    QString username;
    QString nickname;
    QString password;
    QByteArray serializedImage;
};


namespace Ui {
class ProfilePage;
}

class ProfilePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr);
    ~ProfilePage();

private slots:
    void on_buttonBox_ok_cancel_accepted();
    void on_buttonBox_ok_cancel_rejected();
    void on_pushButton_returnToHome_clicked();
    void on_pushButton_changeImage_clicked();
    //void on_nickname_editingFinished();

public slots:
    void onLoadSubscriberInfo(QString username, QString nickname, QByteArray serializedImage);
    void onAccountUpdateError(QString errorMessage);
    void onReloadProfilePageInfo(QString username, QString nickname, QByteArray serializedImage);

private:
    Ui::ProfilePage *ui;
    updateUser_t updateUser;
    void loadImage(QByteArray);
    void resetUpdateUser();
    void resetFields();
    bool managePasswordChange();
    QString nickname_old;

signals:
    void returnToHomeClicked();
    void updateProfileRequest(updateUser_t);
    void resetSubscriberInfo();
};

#endif // PROFILEPAGE_H
