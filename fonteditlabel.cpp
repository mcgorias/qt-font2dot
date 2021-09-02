#include "fonteditlabel.h"
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>

void FontEditLabel::mouseMoveEvent(QMouseEvent *event)
{
    auto x = event->pos().x();
    auto y = event->pos().y();
    if(_Released)
    {
        _Released = false;
        return;
    }

    auto dx = x-_InitialClick.x() ;
    auto dy = y-_InitialClick.y() ;

    emit(moveX(dx));
    emit(moveY(dy));
    _InitialClick = QPoint(x,y);
}

void FontEditLabel::mousePressEvent(QMouseEvent *event)
{
    _Released = true;
    auto x = event->pos().x();
    auto y = event->pos().y();
    _InitialClick = QPoint(x,y);
}

void FontEditLabel::mouseReleaseEvent(QMouseEvent *event)
{
    auto x = event->pos().x();
    auto y = event->pos().y();
    _InitialClick = QPoint(x,y);
}

void FontEditLabel::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;

    if (event->orientation() == Qt::Vertical) {
        emit zoom(numSteps);
    }
    event->accept();

}
