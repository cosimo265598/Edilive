#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QPainter>
#include <profileimage.h>
#include <QWebSocket>

namespace Ui {
class ProfilePage;
}

class ProfilePage : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProfilePage(QWidget *parent = nullptr,QWebSocket *client_socket= nullptr);
    ~ProfilePage();
    void viewDataOfClient(QJsonObject j);


private slots:
    void on_buttonBox_ok_cancel_accepted();

    void on_buttonBox_ok_cancel_rejected();


private:
    Ui::ProfilePage *ui;
    QWebSocket* client_socket;
};

#endif // PROFILEPAGE_H
