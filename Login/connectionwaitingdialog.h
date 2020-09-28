#ifndef CONNECTIONWAITINGDIALOG_H
#define CONNECTIONWAITINGDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionWaitingDialog;
}

class ConnectionWaitingDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionWaitingDialog(QWidget *parent = nullptr);
    ~ConnectionWaitingDialog();
    void setText(QString message);

private:
    Ui::ConnectionWaitingDialog *ui;
};

#endif // CONNECTIONWAITINGDIALOG_H
