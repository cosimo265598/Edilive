#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QStandardItem>
#include <QToolButton>
#include <QStyleOptionButton>
#include <QDebug>
#include <QMouseEvent>

class FileHandler : public QToolButton
{
    Q_OBJECT
public:
    FileHandler(QWidget* parent, const QIcon &icon, const QString &fileName,
                const QString &path, const QString &owner,
                const QString &lastModified, const QString &lastRead);
    ~FileHandler();
private:
    QString path;
    QString owner;
    QString lastModified;
    QString lastRead;

public:

    QString getPath();
    QString getLastRead();
    QString getLastModified();
    QString getOwner();

signals:
    void doubleClicked();
    void clicked();
};

#endif // FILEHANDLER_H
