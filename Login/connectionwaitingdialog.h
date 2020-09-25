#ifndef CONNECTIONWAITINGDIALOG_H
#define CONNECTIONWAITINGDIALOG_H

#include <QWidget>

namespace Ui {
class ConnectionWaitingDialog;
}

class ConnectionWaitingDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionWaitingDialog(QWidget *parent = nullptr);
    ~ConnectionWaitingDialog();

private:
    Ui::ConnectionWaitingDialog *ui;
};

#endif // CONNECTIONWAITINGDIALOG_H
