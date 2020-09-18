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

private:
    Ui::RegistrationDialog *ui;
};

#endif // REGISTRATIONDIALOG_H
