#include "profileimage.h"

ProfileImage::ProfileImage(QWidget *parent) : QLabel(parent){

}

void ProfileImage::paintEvent(QPaintEvent *event)
{

    QPixmap pixmap(":/icons_pack/document_480px.png");
    QPixmap scaled = pixmap.scaled(width(), height(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    QBrush brush(scaled);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, width(), height(), 100, 100);
    QLabel::setStyleSheet("border-radius:90px;background-color:#000000;");
    QLabel::paintEvent(event);
}
