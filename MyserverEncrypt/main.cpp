#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // set your working directory
    QString pathMembro=QDir().homePath();

    const QString pathSaverio("/QtProjects/pds-project/myservertest/MyserverEncrypt/");
    const QString pathCosimo("/GIT/myservertest/MyserverEncrypt/");
    const QString pathSilvia =("/scrivania/ProgettoPds/myservertest/MyserverEncrypt/");

    // set your working directory
    QDir().setCurrent(QDir().homePath()+ "/GIT/myservertest/MyserverEncrypt/");
    if(pathMembro.contains("cosimo"))
        QDir().setCurrent(pathMembro+pathCosimo);
    else if(pathMembro.contains("saverio"))
        QDir().setCurrent(pathMembro+pathSaverio);
    else if(pathMembro.contains("silvia"))// silvia
        QDir().setCurrent(pathMembro+pathSilvia);
    else
        QCoreApplication::exit(-1);

    MainWindow w;
    w.show();
    w.prepareToStart();
    return a.exec();
}
