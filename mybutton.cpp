#include "mybutton.h"
#include "globalvar.h"


MyButton::MyButton(QIcon icon,QToolButton *parent) :
    QToolButton(parent)
{
    on = false;
    disable = false;
    myIcon = icon;
}


void MyButton::mousePressEvent(QMouseEvent *event)
{

    if (!disable)
    {
        if (event->button () == Qt::LeftButton)
        {

            if (group_singleFinish)
            {
                globalButton->setDisable (true);
                globalButton->buttonGotoGray ();
                group_singleFinish = false;
            }
            if (checkOn ())
            {
                QMessageBox::information(0, QString("OOPS"),
                                         QString("Forget to click finish ?"),
                                         QMessageBox::Ok);
                return ;
            }


            if(!disable)
            {
                on  = true;
                QSize size = QSize(120,120);
                QIcon icon;
                QIcon::Mode mode;

                flushButtons();
                globalButton = this;
                targetID = this->text ();
                qDebug () << "on";
                mode = QIcon::Selected;
                image = myIcon.pixmap(size, mode, QIcon::On);
                icon = QIcon(image);
                setIcon (icon);
                setIconSize (size);

            }

        }
    }


}


void MyButton::setDisable (bool setTo)
{
    this->disable = setTo;
}


void MyButton::buttonGotoGray ()
{
    on  = false;
    QSize size = QSize(120,120);
    QIcon icon;
    QIcon::Mode mode;
    qDebug () << "on";
    mode = QIcon::Disabled;
    image = myIcon.pixmap(size, mode, QIcon::On);
    icon = QIcon(image);
    setIcon (icon);
    setIconSize (size);
}

bool MyButton::status ()
{
    return this->disable;
}

void MyButton::flushButtons ()
{
    QSize size = QSize(120,120);
    QIcon icon;
    QIcon::Mode mode;
    foreach(MyButton *item, globalMyButtons)
    {
        if(!item->status ())
        {
            mode = QIcon::Normal;
            image = item->myIcon.pixmap(size, mode, QIcon::On);
            icon = QIcon(image);
            item->setIcon (icon);
            item->setIconSize (size);
        }

    }
}

void MyButton::backToNormal ()
{
    QSize size = QSize(120,120);
    QIcon icon;
    QIcon::Mode mode;
    on = false;
    qDebug () << "back to normal";

    mode = QIcon::Normal;
    image = myIcon.pixmap(size, mode, QIcon::Off);

    icon = QIcon(image);
    setIcon (icon);
    setIconSize (size);
}

void MyButton::setInit ()
{
    on  = true;
    QSize size = QSize(120,120);
    QIcon icon;
    QIcon::Mode mode;

    flushButtons();
    globalButton = this;
    targetID = this->text ();
    qDebug () << "on";
    mode = QIcon::Selected;
    image = myIcon.pixmap(size, mode, QIcon::On);
    icon = QIcon(image);
    setIcon (icon);
    setIconSize (size);
}

bool MyButton::checkOn ()
{
    foreach(MyButton * item, globalMyButtons)
    {
        if (item->ifOn () && (item != this) && confirmTarget)
        {
            return true;
        }
    }
    return false;
}

bool MyButton::ifOn ()
{
    return this->on;
}
