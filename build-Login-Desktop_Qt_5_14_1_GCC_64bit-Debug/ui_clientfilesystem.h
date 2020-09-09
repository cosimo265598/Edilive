/********************************************************************************
** Form generated from reading UI file 'clientfilesystem.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIENTFILESYSTEM_H
#define UI_CLIENTFILESYSTEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClientFilesystem
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QLabel *label_menu;
    QPushButton *pushButton_new_file;
    QPushButton *pushButton_rename_file;
    QPushButton *pushButton_aggiorna_vista;
    QSpacerItem *verticalSpacer;
    QLabel *label_profilo;
    QToolButton *toolButton_profile_page;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout_3;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QLabel *InfoFile_label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ClientFilesystem)
    {
        if (ClientFilesystem->objectName().isEmpty())
            ClientFilesystem->setObjectName(QString::fromUtf8("ClientFilesystem"));
        ClientFilesystem->resize(873, 584);
        centralwidget = new QWidget(ClientFilesystem);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        widget_2->setBaseSize(QSize(0, 0));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(239, 41, 41, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        widget_2->setPalette(palette);
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_menu = new QLabel(widget_2);
        label_menu->setObjectName(QString::fromUtf8("label_menu"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_menu->sizePolicy().hasHeightForWidth());
        label_menu->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(label_menu);

        pushButton_new_file = new QPushButton(widget_2);
        pushButton_new_file->setObjectName(QString::fromUtf8("pushButton_new_file"));
        sizePolicy.setHeightForWidth(pushButton_new_file->sizePolicy().hasHeightForWidth());
        pushButton_new_file->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(pushButton_new_file);

        pushButton_rename_file = new QPushButton(widget_2);
        pushButton_rename_file->setObjectName(QString::fromUtf8("pushButton_rename_file"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_rename_file->sizePolicy().hasHeightForWidth());
        pushButton_rename_file->setSizePolicy(sizePolicy2);

        verticalLayout->addWidget(pushButton_rename_file);

        pushButton_aggiorna_vista = new QPushButton(widget_2);
        pushButton_aggiorna_vista->setObjectName(QString::fromUtf8("pushButton_aggiorna_vista"));
        sizePolicy2.setHeightForWidth(pushButton_aggiorna_vista->sizePolicy().hasHeightForWidth());
        pushButton_aggiorna_vista->setSizePolicy(sizePolicy2);

        verticalLayout->addWidget(pushButton_aggiorna_vista);

        verticalSpacer = new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Preferred);

        verticalLayout->addItem(verticalSpacer);

        label_profilo = new QLabel(widget_2);
        label_profilo->setObjectName(QString::fromUtf8("label_profilo"));
        sizePolicy1.setHeightForWidth(label_profilo->sizePolicy().hasHeightForWidth());
        label_profilo->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(label_profilo);

        toolButton_profile_page = new QToolButton(widget_2);
        toolButton_profile_page->setObjectName(QString::fromUtf8("toolButton_profile_page"));
        sizePolicy.setHeightForWidth(toolButton_profile_page->sizePolicy().hasHeightForWidth());
        toolButton_profile_page->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(toolButton_profile_page);


        horizontalLayout->addWidget(widget_2);

        scrollArea = new QScrollArea(centralwidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy3);
        scrollArea->setLineWidth(1);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 696, 495));
        gridLayout_3 = new QGridLayout(scrollAreaWidgetContents);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(-1, 9, -1, -1);
        widget = new QWidget(scrollAreaWidgetContents);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(-1, 9, -1, -1);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout->setContentsMargins(0, -1, -1, -1);

        gridLayout_2->addLayout(gridLayout, 0, 0, 1, 1);


        gridLayout_3->addWidget(widget, 0, 2, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);


        verticalLayout_2->addLayout(horizontalLayout);

        InfoFile_label = new QLabel(centralwidget);
        InfoFile_label->setObjectName(QString::fromUtf8("InfoFile_label"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(InfoFile_label->sizePolicy().hasHeightForWidth());
        InfoFile_label->setSizePolicy(sizePolicy4);
        InfoFile_label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout_2->addWidget(InfoFile_label);

        ClientFilesystem->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ClientFilesystem);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 873, 22));
        ClientFilesystem->setMenuBar(menubar);
        statusbar = new QStatusBar(ClientFilesystem);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ClientFilesystem->setStatusBar(statusbar);

        retranslateUi(ClientFilesystem);

        QMetaObject::connectSlotsByName(ClientFilesystem);
    } // setupUi

    void retranslateUi(QMainWindow *ClientFilesystem)
    {
        ClientFilesystem->setWindowTitle(QCoreApplication::translate("ClientFilesystem", "ClientFilesystem", nullptr));
        label_menu->setText(QCoreApplication::translate("ClientFilesystem", " Menu Operazioni: ", nullptr));
        pushButton_new_file->setText(QCoreApplication::translate("ClientFilesystem", "  Nuovo File  ", nullptr));
        pushButton_rename_file->setText(QCoreApplication::translate("ClientFilesystem", "Rinomina File", nullptr));
        pushButton_aggiorna_vista->setText(QCoreApplication::translate("ClientFilesystem", "Aggiorna", nullptr));
        label_profilo->setText(QCoreApplication::translate("ClientFilesystem", "Profile Menu:", nullptr));
        toolButton_profile_page->setText(QCoreApplication::translate("ClientFilesystem", "Profilo Personale", nullptr));
        InfoFile_label->setText(QCoreApplication::translate("ClientFilesystem", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClientFilesystem: public Ui_ClientFilesystem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIENTFILESYSTEM_H
