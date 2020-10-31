QT       += core gui
QT       += websockets
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS



# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    SharedFile.cpp \
    Symbol.cpp \
    buildermessageclient.cpp \
    client.cpp \
    connectionwaitingdialog.cpp \
    dialogshare.cpp \
    eventfilterimpl.cpp \
    filehandler.cpp \
    homepage.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindowstacked.cpp \
    presence.cpp \
    profilepage.cpp \
    registrationdialog.cpp \
    startupstackeddialog.cpp \
    subscriber.cpp \
    textedit.cpp \
    updateuser.cpp

HEADERS += \
    SharedFile.h \
    Symbol.h \
    buildermessageclient.h \
    client.h \
    connectionwaitingdialog.h \
    dialogshare.h \
    eventfilterimpl.h \
    filehandler.h \
    homepage.h \
    logindialog.h \
    mainwindowstacked.h \
    presence.h \
    profilepage.h \
    registrationdialog.h \
    startupstackeddialog.h \
    subscriber.h \
    textedit.h \
    updateuser.h

FORMS += \
    connectionwaitingdialog.ui \
    dialogshare.ui \
    homepage.ui \
    logindialog.ui \
    mainwindowstacked.ui \
    profilepage.ui \
    registrationdialog.ui \
    startupstackeddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource_editor.qrc \
    resources_login.qrc
    #resources.qrc
