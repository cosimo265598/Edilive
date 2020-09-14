#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // set your working directory
    const QString pathSaverio = QDir().homePath()+ "/QtProjects/pds-project/myservertest/MyserverEncrypt/";
    const QString pathCosimo = QDir().homePath()+ "/GIT/myservertest/MyserverEncrypt/";
    const QString pathSilvia = QDir().homePath()+ "/GIT/myservertest/MyserverEncrypt/"; // CAMBIA CON IL TUO PATH

    QDir().setCurrent(pathSaverio);
    MainWindow w;
    w.show();
    w.prepareToStart();
    return a.exec();
}
