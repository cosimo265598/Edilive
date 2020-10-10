#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QObject>
#include <QRunnable>

class CreateFile : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit CreateFile(QObject *parent = nullptr);

signals:

};

#endif // CREATEFILE_H
