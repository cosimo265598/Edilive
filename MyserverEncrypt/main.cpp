#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // change working directory
    QDir().setCurrent(QDir().homePath()+"/GIT/myservertest/MyserverEncrypt");
    MainWindow w;
    w.show();
    w.prepareToStart();
    return a.exec();
    // aggiunta di testo.
}
