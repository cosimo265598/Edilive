#ifndef DIALOGSHARE_H
#define DIALOGSHARE_H

#include <QDialog>

namespace Ui {
class DialogShare;
}

class DialogShare : public QDialog
{
    Q_OBJECT

public:
     DialogShare(QWidget *parent = nullptr, QString URI = nullptr);
    ~DialogShare();

private slots:
    void on_pushButton_share_clicked();

    void on_pushButton_cancel_clicked();

signals:
    void shareFile(QString username, QString URI);

private:
    Ui::DialogShare *ui;
    QString URI;
};

#endif // DIALOGSHARE_H
