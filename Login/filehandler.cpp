#include "filehandler.h"

FileHandler::FileHandler(QWidget* parent, const QIcon &icon, const QString &fileName,
                         const QString &URI, const QString &creator,
                         const QString &created, const QString &size)
    : QToolButton()
{
    QToolButton::setIcon(icon);
    QToolButton::setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QToolButton::setText(fileName);
    QToolButton::setIconSize(QSize(100,100));
    QToolButton::setMaximumSize(QSize(100,130));
    QString buttonStyle = "QToolButton{border:none;}\
                            QToolButton:hover{\
                                background-color: rgb(130, 171, 237);\
                            }";

    QToolButton::setStyleSheet(buttonStyle);

    this->URI = URI;
    this->creator = creator;
    this->created = created;
    this->fileName = fileName;
    this->size= size;
}

FileHandler::~FileHandler()
{
    //qDebug()<<"Rimosso"<<this->getPath();
}

QString FileHandler::getURI() const
{
    return URI;
}

QString FileHandler::getSize() const
{
    return size;
}

QString FileHandler::getCreated() const
{
    return created;
}

QString FileHandler::getCreator() const
{
    return creator;
}

QString FileHandler::getFileName() const
{
    return fileName;
}
