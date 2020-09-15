#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QObject>
#include <QWidget>
#include "ui_waitingdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class WaitingDialog; }
QT_END_NAMESPACE

QT_USE_NAMESPACE

class WaitingDialog : public QWidget
{
    Q_OBJECT

public:
    explicit WaitingDialog(QWidget *parent = nullptr);
    ~WaitingDialog();

private:
     Ui::WaitingDialog *ui;

signals:

};

#endif // WAITINGDIALOG_H
