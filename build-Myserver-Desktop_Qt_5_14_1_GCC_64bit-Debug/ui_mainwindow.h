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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionStatus;
    QWidget *centralwidget;
    QLabel *port_label;
    QSpinBox *nport;
    QLabel *status;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *startserver_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *startserver;
    QPushButton *stopserver;
    QWidget *layoutWidget1;
    QHBoxLayout *horizontalLayout;
    QLabel *port_label_2;
    QLCDNumber *num_client;
    QLabel *label;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout_3;
    QPlainTextEdit *commet;
    QPlainTextEdit *commetdb;
    QMenuBar *menubar;
    QMenu *menuDatabase;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(980, 594);
        actionStatus = new QAction(MainWindow);
        actionStatus->setObjectName(QString::fromUtf8("actionStatus"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        port_label = new QLabel(centralwidget);
        port_label->setObjectName(QString::fromUtf8("port_label"));
        port_label->setGeometry(QRect(20, 20, 34, 26));
        nport = new QSpinBox(centralwidget);
        nport->setObjectName(QString::fromUtf8("nport"));
        nport->setGeometry(QRect(70, 20, 161, 26));
        nport->setMinimum(1025);
        nport->setMaximum(65536);
        nport->setValue(1234);
        status = new QLabel(centralwidget);
        status->setObjectName(QString::fromUtf8("status"));
        status->setGeometry(QRect(20, 60, 111, 26));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 510, 951, 41));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        startserver_2 = new QPushButton(layoutWidget);
        startserver_2->setObjectName(QString::fromUtf8("startserver_2"));

        horizontalLayout_2->addWidget(startserver_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        startserver = new QPushButton(layoutWidget);
        startserver->setObjectName(QString::fromUtf8("startserver"));

        horizontalLayout_2->addWidget(startserver);

        stopserver = new QPushButton(layoutWidget);
        stopserver->setObjectName(QString::fromUtf8("stopserver"));

        horizontalLayout_2->addWidget(stopserver);

        layoutWidget1 = new QWidget(centralwidget);
        layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(710, 10, 247, 41));
        horizontalLayout = new QHBoxLayout(layoutWidget1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        port_label_2 = new QLabel(layoutWidget1);
        port_label_2->setObjectName(QString::fromUtf8("port_label_2"));

        horizontalLayout->addWidget(port_label_2);

        num_client = new QLCDNumber(layoutWidget1);
        num_client->setObjectName(QString::fromUtf8("num_client"));
        QFont font;
        font.setBold(true);
        font.setItalic(false);
        font.setWeight(75);
        num_client->setFont(font);
        num_client->setSmallDecimalPoint(false);
        num_client->setDigitCount(5);
        num_client->setSegmentStyle(QLCDNumber::Flat);
        num_client->setProperty("intValue", QVariant(0));

        horizontalLayout->addWidget(num_client);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(500, 60, 71, 16));
        layoutWidget2 = new QWidget(centralwidget);
        layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(20, 90, 951, 391));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        commet = new QPlainTextEdit(layoutWidget2);
        commet->setObjectName(QString::fromUtf8("commet"));

        horizontalLayout_3->addWidget(commet);

        commetdb = new QPlainTextEdit(layoutWidget2);
        commetdb->setObjectName(QString::fromUtf8("commetdb"));
        commetdb->setEnabled(true);
        commetdb->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_3->addWidget(commetdb);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 980, 22));
        menuDatabase = new QMenu(menubar);
        menuDatabase->setObjectName(QString::fromUtf8("menuDatabase"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuDatabase->menuAction());
        menuDatabase->addAction(actionStatus);
        menuDatabase->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionStatus->setText(QCoreApplication::translate("MainWindow", "Status", nullptr));
        port_label->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        status->setText(QCoreApplication::translate("MainWindow", "Status Server:", nullptr));
        startserver_2->setText(QCoreApplication::translate("MainWindow", "Clear", nullptr));
        startserver->setText(QCoreApplication::translate("MainWindow", "START", nullptr));
        stopserver->setText(QCoreApplication::translate("MainWindow", "STOP", nullptr));
        port_label_2->setText(QCoreApplication::translate("MainWindow", "Numero di client collegati", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Database:", nullptr));
        menuDatabase->setTitle(QCoreApplication::translate("MainWindow", "Database", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
