#ifndef REGISTRATIONDIALOG_H
#define REGISTRATIONDIALOG_H

#include <QWidget>
#include <QRegularExpression>
#include <QValidator>

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
    void alreadyAnAccountButtonClicked();
    void registrationRequest(QString username, QString password);
};

#endif // REGISTRATIONDIALOG_H
