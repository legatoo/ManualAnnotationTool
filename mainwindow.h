#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QList>
#include <QMap>
#include <QDebug>
#include <QtXml>
#include <QFile>

#include "usermousefilter.h"
#include "controlform.h"

namespace Ui {
class MainWindow;
}

struct AnnotationUnit
{
    QList<QGraphicsItem *> dotList;
    QGraphicsRectItem *rectangle ;
    QString partName;
    QList<QPointF> userAnnotatePoints;
    QPoint rectCenterPoint;
    int rectWidth;
    int rectHeight;
    qreal rectRealRotationDegree;
    int rectRotationDegree_longer_xAxis;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);


    ~MainWindow();
    
protected:
    void mousePressEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void calculateRectangleParameters();
    void keyPressEvent (QKeyEvent *event);

    int degreeApproximate(qreal line);
    bool saveAnnotationResult_Single();

signals:
    void finishOneGuy_group();

private slots:
    void on_beginButton_clicked();
    void on_finishButton_clicked();
    void on_annotateHEAD_clicked();
    void on_annotateUPARM_clicked();
    void on_annotateFOREARM_clicked();
    void on_annotateBODY_clicked();
    void on_annotateTHIGH_clicked();
    void on_annotateCALF_clicked();
    void saveGroupRecords();

    void on_labelMe_clicked();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    controlForm *controlPanel;

    QList<AnnotationUnit> AnnotationUnits;
    QMap<QString, QList<AnnotationUnit> > groupAnnotationRecord;
    AnnotationUnit annotatingUse;
    QPixmap pixmapItem;
    userMouseFilter *mouseFilter;


    QGraphicsItem *dot;
    QBrush redBrush;
    QPen dotPen;
    QPointF drawDot;
    QBrush blankBrush;
    QPen pen;

    bool group;
    double scaleFactor;
    bool scaled;

    bool beginAnnotation;
    bool finishAnnotation;
    bool partBegin;
    bool partFinish;

    bool headDone;
    bool bodyDone;
    bool armDone;
    bool legDone;

    bool unDo;

};



#endif // MAINWINDOW_H
