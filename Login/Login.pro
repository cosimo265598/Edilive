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
    buildermessageclient.cpp \
    clientfilesystem.cpp \
    eventfilterimpl.cpp \
    filehandler.cpp \
    main.cpp \
    mainwindow.cpp \
    profileimage.cpp \
    profilepage.cpp \
    textedit.cpp \
    waitingdialog.cpp

HEADERS += \
    buildermessageclient.h \
    clientfilesystem.h \
    eventfilterimpl.h \
    filehandler.h \
    mainwindow.h \
    profileimage.h \
    profilepage.h \
    textedit.h \
    waitingdialog.h

FORMS += \
    mainwindow.ui \
    clientfilesystem.ui \
    profilepage.ui \
    waitingdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource_editor.qrc \
    resources_login.qrc
    #resources.qrc
