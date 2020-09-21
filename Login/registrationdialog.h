#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QWidget>

namespace Ui {
class RegistrationDialog;
}

class RegistrationDialog : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationDialog(QWidget *parent = nullptr);
    ~RegistrationDialog();

private slots:


    void on_pushButton_register_clicked();
    void on_pushButton_alreadyAnAccount_clicked();

private:
    Ui::RegistrationDialog *ui;
    void resetPalette();

signals:
    void alreadyAnAccount();
    void registrationRequest(QString username, QString password, QString confirmPassword);
};

#endif // REGISTRATIONDIALOG_H
