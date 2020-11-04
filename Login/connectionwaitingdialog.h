#ifndef CONNECTIONWAITINGDIALOG_H
#define CONNECTIONWAITINGDIALOG_H

#include <QDialog>
#include <QAbstractSocket>
#include <QTimer>

#define MAX_RETRY 3
#define MAX_TIMEOUT 6000

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
    void setNumberRetry();
    int resultOfRetry();
    void stopTimerForced();

public slots:
    void changeState(QAbstractSocket::SocketState state);
    void timeout();

signals:
    void tryToConnectAgain();

private:
    Ui::ConnectionWaitingDialog *ui;
    QTimer timer;
    int number_retry;
};

#endif // CONNECTIONWAITINGDIALOG_H
