#include "filehandler.h"

FileHandler::FileHandler(QWidget* parent, const QIcon &icon, const QString &fileName,
                         const QString &path, const QString &owner,
                         const QString &lastModified, const QString &lastRead)
    : QToolButton()
{
    QToolButton::setIcon(icon);
    QToolButton::setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QToolButton::setText(fileName);
    QToolButton::setIconSize(QSize(100,100));
    QToolButton::setMaximumSize(QSize(100,130));
    QString buttonStyle = "QToolButton{border:none;}";
    QToolButton::setStyleSheet(buttonStyle);

    this->path = path;
    this->owner = owner;
    this->lastModified = lastModified;
    this->lastRead = lastRead;
}

FileHandler::~FileHandler()
{
    qDebug()<<"Rimosso"<<this->getPath();
}

QString FileHandler::getPath(){
    return this->path;
}

QString FileHandler::getLastModified(){
    return this->lastModified;
}

QString FileHandler::getOwner(){
    return this->owner;
}

QString FileHandler::getLastRead(){
    return this->lastRead;
}
