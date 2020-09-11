#ifndef PROFILEIMAGE_H
#define PROFILEIMAGE_H

#include <QLabel>
#include <QPainter>

class ProfileImage : public QLabel
{
    Q_OBJECT
public:
    ProfileImage(QWidget*parent = nullptr);
protected:
    void paintEvent(QPaintEvent *event);

signals:

};

#endif // PROFILEIMAGE_H
