#ifndef USERMOUSEFILTER_H
#define USERMOUSEFILTER_H

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QDebug>

class userMouseFilter : public QObject
{
    Q_OBJECT
public:
    explicit userMouseFilter(QObject *parent = 0);
    QPoint& getUserMousePoint();
    
protected:
    bool eventFilter (QObject *, QEvent *);
    void setUserMousePoint(const QPoint );

signals:
    
public slots:

private:
    QPoint *userMousePoint;
    
};

#endif // USERMOUSEFILTER_H
