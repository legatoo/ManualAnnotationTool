#ifndef CONTROLFORM_H
#define CONTROLFORM_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QDir>
#include <QDebug>

#include "mybutton.h"

namespace Ui {
class controlForm;
}

class controlForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit controlForm(QString fileName,QWidget *parent = 0);
    ~controlForm();

protected:
    //void initPanelPanel();
    void mousePressEvent (QMouseEvent *);

    
private:
    Ui::controlForm *ui;
    MyButton *iconButton;
};

#endif // CONTROLFORM_H
