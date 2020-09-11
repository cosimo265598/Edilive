#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // set your working directory
    QDir().setCurrent(QDir().homePath()+ "/QtProjects/pds-project/myservertest/MyserverEncrypt/");
    MainWindow w;
    w.show();
    w.prepareToStart();
    return a.exec();
}
