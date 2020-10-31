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
                const QString &URI, const QString &creator,
                const QString &created, const int &size);
    ~FileHandler();
private:
    QString fileName;
    QString creator;
    QString created;
    int size;
    QString URI;

public:


    QString getFileName() const;

    QString getCreator() const;

    QString getCreated() const;

    int getSize() const;

    QString getURI() const;

signals:
    void doubleClicked();
    void clicked();
};

#endif // FILEHANDLER_H
