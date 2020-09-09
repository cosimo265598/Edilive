/********************************************************************************
** Form generated from reading UI file 'profilepage.ui'
**
** Created by: Qt User Interface Compiler version 5.14.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILEPAGE_H
#define UI_PROFILEPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProfilePage
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label_username;
    QLabel *username_value;
    QLabel *label_nick;
    QLineEdit *lineEdit_nick;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QLabel *label_pasw;
    QLineEdit *lineEdit_password;
    QLabel *label_confirm_password;
    QLineEdit *lineEdit_confirm_password;
    QLabel *label_imagine_profilo;
    QDialogButtonBox *buttonBox_ok_cancel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ProfilePage)
    {
        if (ProfilePage->objectName().isEmpty())
            ProfilePage->setObjectName(QString::fromUtf8("ProfilePage"));
        ProfilePage->resize(800, 299);
        centralwidget = new QWidget(ProfilePage);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_username = new QLabel(centralwidget);
        label_username->setObjectName(QString::fromUtf8("label_username"));

        verticalLayout->addWidget(label_username);

        username_value = new QLabel(centralwidget);
        username_value->setObjectName(QString::fromUtf8("username_value"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        username_value->setFont(font);
        username_value->setLineWidth(1);

        verticalLayout->addWidget(username_value);

        label_nick = new QLabel(centralwidget);
        label_nick->setObjectName(QString::fromUtf8("label_nick"));

        verticalLayout->addWidget(label_nick);

        lineEdit_nick = new QLineEdit(centralwidget);
        lineEdit_nick->setObjectName(QString::fromUtf8("lineEdit_nick"));

        verticalLayout->addWidget(lineEdit_nick);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(centralwidget);
        line->setObjectName(QString::fromUtf8("line"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(line->sizePolicy().hasHeightForWidth());
        line->setSizePolicy(sizePolicy);
        line->setMaximumSize(QSize(16777215, 3));
        QFont font1;
        font1.setPointSize(16);
        font1.setBold(true);
        font1.setWeight(75);
        line->setFont(font1);
        line->setFrameShadow(QFrame::Raised);
        line->setLineWidth(5);
        line->setFrameShape(QFrame::HLine);

        verticalLayout->addWidget(line);

        label_pasw = new QLabel(centralwidget);
        label_pasw->setObjectName(QString::fromUtf8("label_pasw"));

        verticalLayout->addWidget(label_pasw);

        lineEdit_password = new QLineEdit(centralwidget);
        lineEdit_password->setObjectName(QString::fromUtf8("lineEdit_password"));
        lineEdit_password->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(lineEdit_password);

        label_confirm_password = new QLabel(centralwidget);
        label_confirm_password->setObjectName(QString::fromUtf8("label_confirm_password"));

        verticalLayout->addWidget(label_confirm_password);

        lineEdit_confirm_password = new QLineEdit(centralwidget);
        lineEdit_confirm_password->setObjectName(QString::fromUtf8("lineEdit_confirm_password"));

        verticalLayout->addWidget(lineEdit_confirm_password);


        gridLayout->addLayout(verticalLayout, 0, 0, 2, 1);

        label_imagine_profilo = new QLabel(centralwidget);
        label_imagine_profilo->setObjectName(QString::fromUtf8("label_imagine_profilo"));
        label_imagine_profilo->setScaledContents(true);

        gridLayout->addWidget(label_imagine_profilo, 0, 1, 1, 1);

        buttonBox_ok_cancel = new QDialogButtonBox(centralwidget);
        buttonBox_ok_cancel->setObjectName(QString::fromUtf8("buttonBox_ok_cancel"));
        sizePolicy.setHeightForWidth(buttonBox_ok_cancel->sizePolicy().hasHeightForWidth());
        buttonBox_ok_cancel->setSizePolicy(sizePolicy);
        QFont font2;
        font2.setPointSize(16);
        font2.setBold(false);
        font2.setUnderline(false);
        font2.setWeight(50);
        font2.setStrikeOut(false);
        buttonBox_ok_cancel->setFont(font2);
        buttonBox_ok_cancel->setAutoFillBackground(false);
        buttonBox_ok_cancel->setInputMethodHints(Qt::ImhNone);
        buttonBox_ok_cancel->setOrientation(Qt::Horizontal);
        buttonBox_ok_cancel->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Save);
        buttonBox_ok_cancel->setCenterButtons(true);

        gridLayout->addWidget(buttonBox_ok_cancel, 1, 1, 1, 1);

        ProfilePage->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ProfilePage);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        ProfilePage->setMenuBar(menubar);
        statusbar = new QStatusBar(ProfilePage);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ProfilePage->setStatusBar(statusbar);

        retranslateUi(ProfilePage);

        QMetaObject::connectSlotsByName(ProfilePage);
    } // setupUi

    void retranslateUi(QMainWindow *ProfilePage)
    {
        ProfilePage->setWindowTitle(QCoreApplication::translate("ProfilePage", "MainWindow", nullptr));
        label_username->setText(QCoreApplication::translate("ProfilePage", "Username: ", nullptr));
        username_value->setText(QCoreApplication::translate("ProfilePage", "...", nullptr));
        label_nick->setText(QCoreApplication::translate("ProfilePage", "Nickname: ", nullptr));
        label_pasw->setText(QCoreApplication::translate("ProfilePage", "Nuova Password:", nullptr));
        label_confirm_password->setText(QCoreApplication::translate("ProfilePage", "Conferma Nuova Password: ", nullptr));
        label_imagine_profilo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ProfilePage: public Ui_ProfilePage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILEPAGE_H
