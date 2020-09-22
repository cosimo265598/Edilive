#ifndef STARTUPSTACKEDDIALOG_H
#define STARTUPSTACKEDDIALOG_H

#include <QWidget>

namespace Ui {
class StartupStackedDialog;
}

class StartupStackedDialog : public QWidget
{
    Q_OBJECT

public:
    explicit StartupStackedDialog(QWidget *parent = nullptr);
    ~StartupStackedDialog();

private:
    Ui::StartupStackedDialog *ui;
};

#endif // STARTUPSTACKEDDIALOG_H
