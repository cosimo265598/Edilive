#ifndef EVENTFILTERIMPL_H
#define EVENTFILTERIMPL_H

#include <QObject>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include "filehandler.h"

class EventFilterImpl : public QObject
{
    Q_OBJECT
public:
    explicit EventFilterImpl(QObject *parent = nullptr);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // EVENTFILTERIMPL_H
