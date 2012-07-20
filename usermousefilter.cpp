#include "usermousefilter.h"
#include "globalvar.h"


userMouseFilter::userMouseFilter(QObject *parent) :
    QObject(parent)
{
    userMousePoint = new QPoint();
}

bool userMouseFilter::eventFilter (QObject *object, QEvent *event)
{
    if (event->type () == QEvent::MouseButtonPress)
    {
        QMouseEvent *qmEvent = static_cast<QMouseEvent*>(event);
        setUserMousePoint (qmEvent->pos ());
        return false;
    }
    return false;
}

void userMouseFilter::setUserMousePoint (const QPoint point)
{
    userMousePoint->setX (point.x ());
    userMousePoint->setY (point.y ());
}

QPoint & userMouseFilter::getUserMousePoint ()
{
    return *userMousePoint;
}

