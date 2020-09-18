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
    void on_registration_3_clicked();

private:
    Ui::RegistrationDialog *ui;

signals:
    void alreadyAnAccount();
};

#endif // REGISTRATIONDIALOG_H
