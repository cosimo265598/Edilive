#include "eventfilterimpl.h"

EventFilterImpl::EventFilterImpl(QObject *parent) : QObject(parent){}

bool EventFilterImpl::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonDblClick && static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton ){
        FileHandler* icon = qobject_cast< FileHandler*>( obj );
        qDebug() << "Double";
        emit icon->doubleClicked();
        return true;

    }else if(event->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton){
        qDebug() << "Single";
        FileHandler* icon = qobject_cast<FileHandler*>( obj );
        emit icon->clicked();
        return true;
    }else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}
