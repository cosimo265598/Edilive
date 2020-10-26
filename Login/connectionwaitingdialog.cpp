#include "connectionwaitingdialog.h"
#include "ui_connectionwaitingdialog.h"
#include <QRandomGenerator>
#include <QWebSocket>
#include <QProgressBar>
#include <client.h>



ConnectionWaitingDialog::ConnectionWaitingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectionWaitingDialog)
{
    this->setAttribute(Qt::WA_QuitOnClose);
    ui->setupUi(this);
    this->setModal(false);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    connect(&timer, &QTimer::timeout, this, &ConnectionWaitingDialog::timeout);
    this->number_retry=0;
}

ConnectionWaitingDialog::~ConnectionWaitingDialog()
{
    qDebug()<<"Timer deleted";
    delete ui;
    timer.stop();
}

void ConnectionWaitingDialog::setText(QString message){
    ui->message->setText(message);
}

void ConnectionWaitingDialog::setNumberRetry()
{
    ui->message_retry->setText("Tentativo di riconnessione... "+
                               QString::number(number_retry)+"/"+QString::number(MAX_RETRY));
}

int ConnectionWaitingDialog::resultOfRetry()
{
    return number_retry;
}

void ConnectionWaitingDialog::changeState(QAbstractSocket::SocketState state)
{
    qDebug()<<"Stato socket changed: "<<state;
    switch (state) {
        case QAbstractSocket::UnconnectedState:{
            setText("Lost Connection or server not responding");
            setNumberRetry();
            timer.start(MAX_TIMEOUT);
            this->show();
            break;
        }
        case QAbstractSocket::ConnectedState :{
            qDebug()<<"Connected again";
            timer.stop();
            this->accept();
            this->close();
            number_retry=0;
            ui->progressBar->setValue(0);
            break;
        }
        default:
            return;
    }
}

void ConnectionWaitingDialog::timeout()
{
    number_retry++;
    if(number_retry>MAX_RETRY)
    {
        // no way to to enstablish a new connection
        timer.stop();
        this->reject();
        this->close();
        number_retry=0;
        ui->progressBar->setValue(0);
        qDebug()<<"Stop timer";
    }
    else{
            ui->progressBar->setValue((100/MAX_RETRY)*number_retry +1);
            setNumberRetry();
            emit tryToConnectAgain();
    }
}
