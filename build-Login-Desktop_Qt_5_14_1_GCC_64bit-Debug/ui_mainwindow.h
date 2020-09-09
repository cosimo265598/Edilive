/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *user;
    QLabel *label_2;
    QLineEdit *psw;
    QLabel *labelconfpsw;
    QLineEdit *confpsw;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QPushButton *login;
    QPushButton *sign;
    QPushButton *sign_account;
    QPushButton *back;
    QLabel *status;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(723, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(120, 70, 451, 251));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        user = new QLineEdit(verticalLayoutWidget);
        user->setObjectName(QString::fromUtf8("user"));

        verticalLayout->addWidget(user);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        psw = new QLineEdit(verticalLayoutWidget);
        psw->setObjectName(QString::fromUtf8("psw"));

        verticalLayout->addWidget(psw);

        labelconfpsw = new QLabel(verticalLayoutWidget);
        labelconfpsw->setObjectName(QString::fromUtf8("labelconfpsw"));
        labelconfpsw->setEnabled(true);

        verticalLayout->addWidget(labelconfpsw);

        confpsw = new QLineEdit(verticalLayoutWidget);
        confpsw->setObjectName(QString::fromUtf8("confpsw"));
        confpsw->setEnabled(true);

        verticalLayout->addWidget(confpsw);

        verticalLayoutWidget_2 = new QWidget(centralwidget);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(220, 340, 231, 161));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        login = new QPushButton(verticalLayoutWidget_2);
        login->setObjectName(QString::fromUtf8("login"));
        login->setAutoFillBackground(true);

        verticalLayout_2->addWidget(login);

        sign = new QPushButton(verticalLayoutWidget_2);
        sign->setObjectName(QString::fromUtf8("sign"));

        verticalLayout_2->addWidget(sign);

        sign_account = new QPushButton(verticalLayoutWidget_2);
        sign_account->setObjectName(QString::fromUtf8("sign_account"));

        verticalLayout_2->addWidget(sign_account);

        back = new QPushButton(centralwidget);
        back->setObjectName(QString::fromUtf8("back"));
        back->setGeometry(QRect(500, 10, 71, 41));
        status = new QLabel(centralwidget);
        status->setObjectName(QString::fromUtf8("status"));
        status->setGeometry(QRect(0, 540, 721, 16));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 723, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Username:", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Password:", nullptr));
        labelconfpsw->setText(QCoreApplication::translate("MainWindow", "Conferma Password", nullptr));
#if QT_CONFIG(tooltip)
        login->setToolTip(QString());
#endif // QT_CONFIG(tooltip)
        login->setText(QCoreApplication::translate("MainWindow", "Login", nullptr));
        sign->setText(QCoreApplication::translate("MainWindow", "Registrati", nullptr));
        sign_account->setText(QCoreApplication::translate("MainWindow", "Create Account", nullptr));
        back->setText(QCoreApplication::translate("MainWindow", "<", nullptr));
        status->setText(QCoreApplication::translate("MainWindow", " Status: Not connected.", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
