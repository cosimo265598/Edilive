#ifndef PROFILEPAGE_H
#define PROFILEPAGE_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QPainter>
#include <profileimage.h>

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


private:
    Ui::ProfilePage *ui;
};

#endif // PROFILEPAGE_H
