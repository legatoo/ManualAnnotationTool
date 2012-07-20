#ifndef MYBUTTON_H
#define MYBUTTON_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QDebug>

class MyButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MyButton(QIcon icon,QToolButton *parent = 0);
    void setInit();
    bool ifOn();
protected:
    void mousePressEvent(QMouseEvent *event);
    void buttonGotoGray();
    void setDisable(bool);
    bool status();
    void flushButtons();
    bool checkOn();

signals:
    
public slots:
    void backToNormal();

private:
    bool on;
    bool disable;
    QIcon myIcon;
    QPixmap image;

};

extern QString targetID;

#endif // MYBUTTON_H
