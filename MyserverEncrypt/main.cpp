#include "mainwindow.h"
#include "serverexception.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("CoopTextEditor - ServerSide");
    try
    {
        QString pathMembro=QDir().homePath();
        // set your working directory
        const QString pathSaverio("/QtProjects/pds-project/myservertest/MyserverEncrypt/");
        const QString pathCosimo("/GIT_Saverio/myservertest/MyserverEncrypt/");
        const QString pathSilvia =("/scrivania/ProgettoPds/myservertest/MyserverEncrypt/");

        // set your working directory
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
    }catch (StartupException& ex) {
        qCritical().nospace() << endl << ex.what();
        return EXIT_FAILURE;
    }
    catch (ServerException& ex)
    {
        // nothing to do - close application
        qCritical().nospace() << endl << ex.what();
        return EXIT_FAILURE;
    }

}
