#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globalvar.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    blankBrush = QBrush(Qt::transparent);
    pen = QPen(Qt::red);
    pen.setWidth(1.5);
    redBrush = QBrush(Qt::red);
    dotPen = QPen(Qt::red);
    dotPen.setWidth (3);


    beginAnnotation = false;
    finishAnnotation = false;
    partBegin = false;
    partFinish = false;
    scaled = false;
    unDo = false;
    group = false;
    ui->graphicsView->setAcceptDrops (true);
    mouseFilter = new userMouseFilter();
    ui->graphicsView->installEventFilter ( mouseFilter);
    ui->beginButton->hide ();
    scaleFactor = 1.0;

    ui->statusBar->showMessage ("A Steven YANG Product.");

    groupSave = new QPushButton();
    groupSave->setText ("Save All");

    ui->verticalLayout->addWidget (groupSave);
    //beautiful connect!
    connect (groupSave, SIGNAL(clicked()), this, SLOT(saveGroupRecords()));



}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton)&& beginAnnotation) {
        if (targetID.isEmpty ())
        {
            QMessageBox::information(0, QString("OOPS"),
                                     QString("Please choose one target to annotate."),
                                     QMessageBox::Ok);
            return;
        }
        if (annotatingUse.userAnnotatePoints.size () != 3 )
            partBegin = true;
        if (annotatingUse.userAnnotatePoints.size () == 3)
            partFinish = true;
        //the point based on image coordinate system
        const QPointF mousePoint = ui->graphicsView->mapToScene (
                   mouseFilter->getUserMousePoint ());

        if (partBegin && !partFinish)
        {
            //qDebug () << mousePoint;
            drawDot = mousePoint;
            annotatingUse.dotList.append (scene->addEllipse (mousePoint.x (),mousePoint.y (),1.0,1.0,dotPen,redBrush));
            annotatingUse.userAnnotatePoints.append (mousePoint);
            confirmTarget = true;
        }
        if (partBegin && partFinish)
        {
            if (annotatingUse.userAnnotatePoints[0].y () <= annotatingUse.userAnnotatePoints[1].y ()
                    || annotatingUse.userAnnotatePoints[0].y () <= annotatingUse.userAnnotatePoints[2].y ())
            {
                annotatingUse.userAnnotatePoints.clear ();
                partBegin = false;
                partFinish = false;
                unDo = false;
                foreach(QGraphicsItem *dot,annotatingUse.dotList)
                    dot->hide ();
                QMessageBox::information(0, QString("notification"),
                                         QString("Sorry! Please begin at the buttom-left point of the rectangle."),
                                         QMessageBox::Ok);
                return ;
            }
            calculateRectangleParameters ();

//            foreach(QPointF debugUse, annotatingUse.userAnnotatePoints)
//                qDebug () << debugUse << "*";

            annotatingUse.rectangle = scene->addRect(
                        annotatingUse.userAnnotatePoints[1].x (),
                        annotatingUse.userAnnotatePoints[1].y (),
                        annotatingUse.rectWidth,
                        annotatingUse.rectHeight,
                        pen,
                        blankBrush);
            //annotatingUse.rectangle->setFlag(QGraphicsItem::ItemIsMovable);

            //I don't why, but this line make QGraphicsItem can accept leftMousePressEvent
            //I guess this make the item only accept right button, and let left button go, so
            //the View &Scene can receive button.
            annotatingUse.rectangle->setAcceptedMouseButtons (Qt::RightButton);
            //rotate with the top-left point
            annotatingUse.rectangle->setTransform(QTransform().translate(
                                   annotatingUse.userAnnotatePoints[1].x (),
                                   annotatingUse.userAnnotatePoints[1].y ()).rotate(
                                   annotatingUse.rectRealRotationDegree).translate(
                                                                                    -annotatingUse.userAnnotatePoints[1].x (),
                                                                                    -annotatingUse.userAnnotatePoints[1].y ()
                                                                                   )
                                    );

            if (scaled)
            {
                qDebug () << scaleFactor;
                qDebug () << annotatingUse.rectWidth << "    "<<annotatingUse.rectHeight;
                annotatingUse.rectHeight *= scaleFactor;
                annotatingUse.rectWidth *= scaleFactor;
                qDebug () << annotatingUse.rectWidth << "    "<<annotatingUse.rectHeight;

                qDebug () << "center" << annotatingUse.rectCenterPoint;
                annotatingUse.rectCenterPoint.setX (annotatingUse.rectCenterPoint.x ()*scaleFactor);
                annotatingUse.rectCenterPoint.setY (annotatingUse.rectCenterPoint.y ()*scaleFactor);
                qDebug () << "center" << annotatingUse.rectCenterPoint;

                foreach (QPointF point,annotatingUse.userAnnotatePoints)
                {
                    point.setX (point.x ()*scaleFactor);
                    point.setY (point.y ()*scaleFactor);
                }

            }


            //store the information of this part in list
            AnnotationUnits.append (annotatingUse);
            if (annotatingUse.partName == "head")
                head++;
            if(annotatingUse.partName == "body")
                body++;
            if (annotatingUse.partName == "uparm")
                up++;
            if (annotatingUse.partName == "forearm")
                fore++;
            if (annotatingUse.partName == "thigh")
                thigh++;
            if (annotatingUse.partName == "calf")
                calf++;

            annotatingUse.userAnnotatePoints.clear ();
            partBegin = false;
            partFinish = false;
            unDo = true;
        }

    }
}


void MainWindow::dragEnterEvent(QDragEnterEvent  *event)
{
    if (!beginAnnotation)
    {
        if (event->mimeData()->hasFormat("text/uri-list"))
        {
            event->acceptProposedAction();
            event->accept();
        }
        else
            event->ignore();
    }
}


void MainWindow::dropEvent(QDropEvent  *event)
{

    if (!beginAnnotation)
    {

        QList<QUrl> urls = event->mimeData()->urls();
        if (urls.isEmpty()) {
            qDebug () << tr("file path is empty.");
            return;
        }

        fileName = urls.first().toLocalFile();
        if (fileName.isEmpty()) {
            return;
        }
        QFile file(fileName);
        if(file.open(QFile::ReadOnly)){
            QByteArray data=file.readAll ();


            if (fileName.split ("/").last ().split (".")[1] == "png")
            {
                qDebug () <<fileName.split ("/").last ().split (".")[1] ;
                pixmapItem.loadFromData (data,"png");
            }
            if (fileName.split ("/").last ().split (".")[1] == "jpg")
            {
                qDebug () <<"enter";
                qDebug () <<fileName.split ("/").last ().split (".")[1] ;
                pixmapItem.loadFromData (data,"jpeg");
            }


            if(fileName.split ("/").last ().left (1) == "S")
            {

                targetID = fileName.split ("/").last ();
                //annotatingUse.partName = globalfileName::fileName;
                beginAnnotation = true;
                ui->annotateHEAD->setChecked (true);
                annotatingUse.partName = "head";
                pen = QPen(Qt::red);
                groupSave->hide ();

            }
            else
            {
                beginAnnotation = true;
                group = true;
                ui->annotateHEAD->setChecked (true);
                annotatingUse.partName = "head";
                pen = QPen(Qt::red);
                controlPanel = new controlForm(fileName);
                groupSave->show();
                controlPanel->show ();

            }

            scene = new QGraphicsScene();
            qreal widthOfPixmapItem = pixmapItem.width ();
            qreal heightOfPixmapItem = pixmapItem.height ();

            orignalImageWidth = widthOfPixmapItem;
            orignalImageHeight = heightOfPixmapItem;

            if (widthOfPixmapItem < ui->graphicsView->width ()
                    && heightOfPixmapItem < ui->graphicsView->height ())
            {

                scene->addPixmap (pixmapItem);
                ui->graphicsView->setScene (scene);
            }
            else
            {
                qDebug () << "I am a big image!";
                scaled = true;
                pixmapItem = pixmapItem.scaled(ui->graphicsView->size (),Qt::KeepAspectRatio,Qt::FastTransformation);
                scaleFactor = widthOfPixmapItem/pixmapItem.width ();
                scene->addPixmap (pixmapItem);
                ui->graphicsView->setScene (scene);


            }

        }
    }

}

void MainWindow::calculateRectangleParameters ()
{
    QLineF *lineBetween_points_1_and_2 = new QLineF(
                annotatingUse.userAnnotatePoints[0].x (),
                annotatingUse.userAnnotatePoints[0].y (),
                annotatingUse.userAnnotatePoints[1].x (),
                annotatingUse.userAnnotatePoints[1].y ()
            );
    QLineF *lineBetween_points_2_and_3 = new QLineF(
                annotatingUse.userAnnotatePoints[1].x (),
                annotatingUse.userAnnotatePoints[1].y (),
                annotatingUse.userAnnotatePoints[2].x (),
                annotatingUse.userAnnotatePoints[2].y ()
            );
    //if the user draws the longer line, we easily get the rotation
    if (lineBetween_points_1_and_2->length () >= lineBetween_points_2_and_3->length ())
    {
        annotatingUse.rectRotationDegree_longer_xAxis = degreeApproximate (lineBetween_points_1_and_2->angle ());
    }

    //height
    annotatingUse.rectHeight = lineBetween_points_1_and_2->length ();
    //width
    annotatingUse.rectWidth = lineBetween_points_2_and_3->length ();

    qreal rotationDegree = abs(90 - lineBetween_points_1_and_2->angle ());

    if (annotatingUse.userAnnotatePoints[1].x () >= annotatingUse.userAnnotatePoints[0].x ())
        //ratation
        annotatingUse.rectRealRotationDegree = rotationDegree;
    else
        annotatingUse.rectRealRotationDegree = -rotationDegree;


    lineBetween_points_1_and_2->setP1 (annotatingUse.userAnnotatePoints[1]);
    lineBetween_points_1_and_2->setP2 (annotatingUse.userAnnotatePoints[0]);

    QLineF tempLine = lineBetween_points_1_and_2->normalVector ();
    tempLine.setLength (annotatingUse.rectWidth);

    //rotation degree : The longer line, counter-clockwise, x axis(x>0).
    if (lineBetween_points_1_and_2->length () < lineBetween_points_2_and_3->length ())
    {
//        if (tempLine.angle () > 180)
//            annotatingUse.rectRotationDegree_longer_xAxis = degreeApproximate (tempLine.angle ()-180);
//        else
//            annotatingUse.rectRotationDegree_longer_xAxis = degreeApproximate (tempLine.angle ());
        annotatingUse.rectRotationDegree_longer_xAxis = 360 - lineBetween_points_1_and_2->angle ();
    }

    qDebug () << "line 1 length: " << lineBetween_points_1_and_2->length () << "angle:" << lineBetween_points_1_and_2->angle ();
    qDebug () << "line 2 length: " << lineBetween_points_2_and_3->length () << "angle:" << lineBetween_points_2_and_3->angle ();
    qDebug () << "tempLine length " << tempLine.length () << "angle" << tempLine.angle ();
    qDebug () << "rotation :" << annotatingUse.rectRotationDegree_longer_xAxis;


    //centerPoint
    QLineF *tempLine2 = new QLineF( annotatingUse.userAnnotatePoints[0].x (),
                                    annotatingUse.userAnnotatePoints[0].y (),
                                    tempLine.pointAt (1).x (),
                                    tempLine.pointAt (1).y ()
                                   );
    annotatingUse.rectCenterPoint = tempLine2->pointAt (0.5).toPoint ();

}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_beginButton_clicked()
{
    beginAnnotation = true;
    if (!group)
    {
        ui->annotateHEAD->setChecked (true);
        annotatingUse.partName = "head";
        qDebug () << annotatingUse.partName;
        QMessageBox::information(0, QString("notification"),
                                 QString("Let's annotate!"),
                                 QMessageBox::Ok);
    }
    else
    {
        ui->annotateHEAD->setChecked (true);
        annotatingUse.partName = "head";
        controlPanel->show ();
    }

}

void MainWindow::on_finishButton_clicked()
{


    qDebug () << tr("target ID: ") << targetID;
//    foreach (const AnnotationUnit &annotatedPart,AnnotationUnits)
//    {
//        qDebug () << tr("part name: ") << annotatedPart.partName;
//        qDebug () << tr("width: ") << annotatedPart.rectWidth;
//        qDebug () << tr("height") << annotatedPart.rectHeight;
//        qDebug () << tr("rotation degree (internal): ") << annotatedPart.rectRealRotationDegree;
//        qDebug () << tr("rotation degree: ") << annotatedPart.rectRotationDegree_longer_xAxis;
//        qDebug () << tr("center point: ") << annotatedPart.rectCenterPoint;
//        qDebug () << tr("----------------------------");
//    }
    if (!group)
    {

        if (saveAnnotationResult_Single ())
        {
            AnnotationUnits.clear ();

            QMessageBox::information(0, QString("notification"),
                                     QString("Great job! Result is stored in file.\nYou can drag another image."),
                                     QMessageBox::Ok);
            head = 0;
            body = 0;
            up = 0;
            fore = 0;
            thigh = 0;
            calf = 0;
            unDo = false;
        }
        else
        {
            AnnotationUnits.clear ();
            QMessageBox::information(0, QString("OOPS"),
                                     QString("Error reason: \n1. I can't create file to write.\n2. You annotate unnaturally?\n I mean more than one head or something like this??\nPlease LOAD this image AGAIN."),
                                     QMessageBox::Ok);
        }


    }
    else
    {
        if (!AnnotationUnits.isEmpty ())
        {
            //group save!
            group_singleFinish = true;


            ui->annotateHEAD->setChecked (true);
            annotatingUse.partName = "head";
            pen = QPen(Qt::red);

            qDebug () << targetID;
            groupAnnotationRecord.insert (targetID, AnnotationUnits);
            foreach(AnnotationUnit item, groupAnnotationRecord[targetID])
            {
                qDebug () << tr("part name: ") << item.partName;
                qDebug () << tr("width: ") << item.rectWidth;
                qDebug () << tr("height") << item.rectHeight;
                qDebug () << tr("rotation degree (internal): ") << item.rectRealRotationDegree;
                qDebug () << tr("rotation degree: ") << item.rectRotationDegree_longer_xAxis;
                qDebug () << tr("center point: ") << item.rectCenterPoint;
            }
            AnnotationUnits.clear ();
            targetID.clear ();
            confirmTarget = false;
            head = 0;
            body = 0;
            up = 0;
            fore = 0;
            thigh = 0;
            calf = 0;
            unDo = false;
        }
        else
        {
            //AnnotationUnits.clear ();
            QMessageBox::information(0, QString("OOPS"),
                                     QString("You haven't annotated yet. Please annotate something. "),
                                     QMessageBox::Ok);
        }
    }
}

void MainWindow::keyPressEvent (QKeyEvent *event)
{
    qDebug () << unDo;
    if (event->key () == Qt::Key_Escape && beginAnnotation && unDo)
    {
        unDo = false;
        qDebug () <<"head number: " << head;
        if (!AnnotationUnits.empty ())
        {
            if (AnnotationUnits.last ().partName == "head")
                head --;
            if (AnnotationUnits.last ().partName == "body")
                body --;
            if (AnnotationUnits.last ().partName == "uparm")
                up --;
            if (AnnotationUnits.last ().partName == "forearm")
                fore --;
            if (AnnotationUnits.last ().partName == "thigh")
                thigh --;
            if (AnnotationUnits.last ().partName == "calf")
                calf --;

            AnnotationUnits.removeLast ();
        }

        annotatingUse.rectangle->hide ();
        foreach(QGraphicsItem *dot,annotatingUse.dotList)
            dot->hide ();
        qDebug () << tr("delete");


    }
}

int MainWindow::degreeApproximate (qreal degree)
{
    int returnRotation;
    double remainder = (int)degree % 15;

    if (remainder >= 7.5)
        returnRotation = 15 * ((int)(degree/15) + 1);
    else
        returnRotation = 15 * (int)(degree/15);

    return returnRotation;
}

bool MainWindow::saveAnnotationResult_Single ()
{
    if (!AnnotationUnits.isEmpty ())
    {
        beginAnnotation = false;
        scaled = false;
        QString path = "result/";
        QFile resultFile(path + targetID.split (".")[0] + ".xml");

        if (!resultFile.open (QIODevice::WriteOnly | QIODevice::Truncate))
        {
            return false;
        }
        QDomDocument doc;
        QDomText text;
        QDomElement element;

        QDomElement centerNode;
        QDomElement heightNode;
        QDomElement widthNode;
        QDomElement rotationNode;

        QString center;
        QString width;
        QString height;
        QString rotation;

        int LRU = 0;
        int LRF = 0;
        int LRT = 0;
        int LRC = 0;


        QDomProcessingInstruction instruction;

        instruction = doc.createProcessingInstruction ("xml", "version = \"1.0\" encoding = \"UTF-8\"");
        doc.appendChild (instruction);

        QDomElement target = doc.createElement (targetID.split (".")[0]);

        QDomText imageInfo;

        QDomElement imageSize = doc.createElement ("imagesize");
        QDomElement imageWidth = doc.createElement ("imageWidth");
        QDomElement imageHeight = doc.createElement ("imageHeight");

        imageInfo = doc.createTextNode (QString::number (orignalImageWidth));
        imageWidth.appendChild (imageInfo);
        imageInfo = doc.createTextNode (QString::number (orignalImageHeight));
        imageHeight.appendChild (imageInfo);

        imageSize.appendChild (imageWidth);
        imageSize.appendChild (imageHeight);

        target.appendChild (imageSize);


        QDomElement uparmElement = doc.createElement ("uparm");
        QDomElement foreArmElement = doc.createElement ("forearm");
        QDomElement thighElement = doc.createElement ("thigh");
        QDomElement calfElement = doc.createElement ("calf");



        for (int i = 0; i < AnnotationUnits.size (); i++)
        {
            centerNode = doc.createElement ("center");
            heightNode = doc.createElement ("height");
            widthNode = doc.createElement ("width");
            rotationNode = doc.createElement ("rotation");

            center = (QString)("(") + QString::number (AnnotationUnits[i].rectCenterPoint.x ()) + (QString)(",") +
                    QString::number (AnnotationUnits[i].rectCenterPoint.y ()) + (QString)(")");
            width = QString::number (AnnotationUnits[i].rectWidth);
            height = QString::number (AnnotationUnits[i].rectHeight);
            rotation = QString::number (AnnotationUnits[i].rectRotationDegree_longer_xAxis);


            text = doc.createTextNode (center);
            centerNode.appendChild (text);
            text = doc.createTextNode (width);
            widthNode.appendChild (text);
            text = doc.createTextNode (height);
            heightNode.appendChild (text);
            text = doc.createTextNode (rotation);
            rotationNode.appendChild (text);

            if(AnnotationUnits[i].partName == "head" || AnnotationUnits[i].partName == "body")
            {
                if (head > 1 || body >1 )
                {
                    return false;
                }

                element =  doc.createElement (AnnotationUnits[i].partName);

                element.appendChild (centerNode);
                element.appendChild (widthNode);
                element.appendChild (heightNode);
                element.appendChild (rotationNode);
                target.appendChild (element);

            }
            else{

                if (AnnotationUnits[i].partName == "uparm")
                {
                    if (up > 2)
                        return false;

                    element = doc.createElement (limbs[LRU]);
                    LRU++;
                    element.appendChild (centerNode);
                    element.appendChild (widthNode);
                    element.appendChild (heightNode);
                    element.appendChild (rotationNode);
                    uparmElement.appendChild (element);
                    target.appendChild (uparmElement);

                }

                if (AnnotationUnits[i].partName == "forearm")
                {
                    if (fore > 2)
                        return false;


                    element = doc.createElement (limbs[LRF]);
                    LRF++;
                    element.appendChild (centerNode);
                    element.appendChild (widthNode);
                    element.appendChild (heightNode);
                    element.appendChild (rotationNode);
                    foreArmElement.appendChild (element);
                    target.appendChild (foreArmElement);

                }

                if (AnnotationUnits[i].partName == "thigh")
                {
                    if (thigh > 2)
                        return false;


                    element = doc.createElement (limbs[LRT]);
                    LRT++;
                    element.appendChild (centerNode);
                    element.appendChild (widthNode);
                    element.appendChild (heightNode);
                    element.appendChild (rotationNode);
                    thighElement.appendChild (element);
                    target.appendChild (thighElement);
                }

                if (AnnotationUnits[i].partName == "calf")
                {
                    if (calf > 2)
                        return false;

                    element = doc.createElement (limbs[LRC]);
                    LRC++;
                    element.appendChild (centerNode);
                    element.appendChild (widthNode);
                    element.appendChild (heightNode);
                    element.appendChild (rotationNode);
                    calfElement.appendChild (element);
                    target.appendChild (calfElement);
                }

            }

        }

        doc.appendChild (target);

        QTextStream out(&resultFile);
        out.setCodec("UTF-8");
        doc.save (out,4, QDomNode::EncodingFromTextStream);
        targetID.clear ();



        return true;
    }
    else
    {
        QMessageBox::information(0, QString("OOPS"),
                                 QString("You haven't annotate yet."),
                                 QMessageBox::Ok);
        return false;
    }


}

void MainWindow::on_annotateHEAD_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::red);
        annotatingUse.partName = "head";
    }
}

void MainWindow::on_annotateUPARM_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::cyan);
        annotatingUse.partName = "uparm";
    }
}

void MainWindow::on_annotateFOREARM_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::green);
        annotatingUse.partName = "forearm";
    }
}

void MainWindow::on_annotateBODY_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::blue);
        annotatingUse.partName = "body";
    }
}

void MainWindow::on_annotateTHIGH_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::yellow);
        annotatingUse.partName = "thigh";
    }
}

void MainWindow::on_annotateCALF_clicked()
{
    if (beginAnnotation)
    {
        pen = QPen(Qt::magenta);
        annotatingUse.partName = "calf";
    }
}

void MainWindow::saveGroupRecords ()
{
    qDebug () << "----------------now save group records------------------";

    if (!AnnotationUnits.isEmpty ())
    {
        QMessageBox::information(0, QString("OOPS"),
                                 QString("Forget to click finish ?"),
                                 QMessageBox::Ok);
        return ;
    }

    if (!groupAnnotationRecord.isEmpty ())
    {
        beginAnnotation = false;
        group = false;
        scaled = false;
        QString sceneID = scene_ID;

        //fileName is sceneID.
        QString path = "result/";
        QFile resultFile(path + sceneID + ".xml");

        if (!resultFile.open (QIODevice::WriteOnly | QIODevice::Truncate))
        {
            QMessageBox::information(0, QString("OOPS"),
                                     QString("Can't create file. Make sure you have choosen one target to annotate."),
                                     QMessageBox::Ok);
            controlPanel->hide ();
            return ;
        }

        QDomDocument doc;
        QDomElement scene;
        QDomText text;
        QDomElement element;



        QDomElement centerNode;
        QDomElement heightNode;
        QDomElement widthNode;
        QDomElement rotationNode;
        QDomElement target;

        QString center;
        QString width;
        QString height;
        QString rotation;


        QDomProcessingInstruction instruction;
        instruction = doc.createProcessingInstruction ("xml", "version = \"1.0\" encoding = \"UTF-8\"");
        doc.appendChild (instruction);
        scene = doc.createElement (sceneID);

        QDomText imageInfo;

        QDomElement imageSize = doc.createElement ("imagesize");
        QDomElement imageWidth = doc.createElement ("imageWidth");
        QDomElement imageHeight = doc.createElement ("imageHeight");

        imageInfo = doc.createTextNode (QString::number (orignalImageWidth));
        imageWidth.appendChild (imageInfo);
        imageInfo = doc.createTextNode (QString::number (orignalImageHeight));
        imageHeight.appendChild (imageInfo);

        imageSize.appendChild (imageWidth);
        imageSize.appendChild (imageHeight);

        scene.appendChild (imageSize);//-----------scene

        QMapIterator<QString, QList<AnnotationUnit> > iter(groupAnnotationRecord);


        /*-------------------iter the QMap-----------------------*/
        while (iter.hasNext())
        {
             iter.next();
             qDebug() << iter.key() ;

             target = doc.createElement (iter.key ());

             QDomElement uparmElement = doc.createElement ("uparm");
             QDomElement foreArmElement = doc.createElement ("forearm");
             QDomElement thighElement = doc.createElement ("thigh");
             QDomElement calfElement = doc.createElement ("calf");

             int LRU = 0;
             int LRF = 0;
             int LRT = 0;
             int LRC = 0;


             //targetID

             /*------------------iter the parts-------------------*/
             foreach(AnnotationUnit item, iter.value ())                                    //parts
             {

                 centerNode = doc.createElement ("center");
                 heightNode = doc.createElement ("height");
                 widthNode = doc.createElement ("width");
                 rotationNode = doc.createElement ("rotation");


                 center = (QString)("(") + QString::number (item.rectCenterPoint.x ()) + (QString)(",") +
                         QString::number (item.rectCenterPoint.y ()) + (QString)(")");
                 width = QString::number (item.rectWidth);
                 height = QString::number (item.rectHeight);
                 rotation = QString::number (item.rectRotationDegree_longer_xAxis);


                 text = doc.createTextNode (center);
                 centerNode.appendChild (text);
                 text = doc.createTextNode (width);
                 widthNode.appendChild (text);
                 text = doc.createTextNode (height);
                 heightNode.appendChild (text);
                 text = doc.createTextNode (rotation);
                 rotationNode.appendChild (text);


                 if(item.partName == "head" || item.partName == "body")
                 {
                     if (head > 1 || body >1 )
                     {
                         QMessageBox::information(0, QString("OOPS"),
                                        QString("Error reason: Two head? Do you need some COFFEE?\nPlease LOAD this image AGAIN."),
                                        QMessageBox::Ok);
                         groupAnnotationRecord.clear ();
                         scene_ID.clear ();
                         controlPanel->hide ();
                         return;
                     }

                     element =  doc.createElement (item.partName);

                     element.appendChild (centerNode);
                     element.appendChild (widthNode);
                     element.appendChild (heightNode);
                     element.appendChild (rotationNode);
                     target.appendChild (element);

                 }
                 else{

                     if (item.partName == "uparm")
                     {
                         if (up >2)
                         {
                             QMessageBox::information(0, QString("OOPS"),
                                            QString("Error reason: \n2. Three uparm? Do you need some COFFEE?Please LOAD this image AGAIN."),
                                            QMessageBox::Ok);
                             groupAnnotationRecord.clear ();
                             controlPanel->hide ();
                             scene_ID.clear ();
                             return;
                         }

                         element = doc.createElement (limbs[LRU]);
                         LRU++;
                         element.appendChild (centerNode);
                         element.appendChild (widthNode);
                         element.appendChild (heightNode);
                         element.appendChild (rotationNode);
                         uparmElement.appendChild (element);
                         target.appendChild (uparmElement);

                     }

                     if (item.partName == "forearm")
                     {
                         if (fore >2)
                         {
                             QMessageBox::information(0, QString("OOPS"),
                                            QString("Error reason: \n2. Three forearm? Do you need some COFFEE?Please LOAD this image AGAIN."),
                                            QMessageBox::Ok);
                             groupAnnotationRecord.clear ();
                             controlPanel->hide ();
                             scene_ID.clear ();
                             return;
                         }

                         element = doc.createElement (limbs[LRF]);
                         LRF++;
                         element.appendChild (centerNode);
                         element.appendChild (widthNode);
                         element.appendChild (heightNode);
                         element.appendChild (rotationNode);
                         foreArmElement.appendChild (element);
                         target.appendChild (foreArmElement);

                     }

                     if (item.partName == "thigh")
                     {
                         if (thigh >2)
                         {
                             QMessageBox::information(0, QString("OOPS"),
                                            QString("Error reason: \n2. Three thigh? Do you need some COFFEE?Please LOAD this image AGAIN."),
                                            QMessageBox::Ok);
                             groupAnnotationRecord.clear ();
                             scene_ID.clear ();
                             controlPanel->hide ();
                             return;
                         }

                         element = doc.createElement (limbs[LRT]);
                         LRT++;
                         element.appendChild (centerNode);
                         element.appendChild (widthNode);
                         element.appendChild (heightNode);
                         element.appendChild (rotationNode);
                         thighElement.appendChild (element);
                         target.appendChild (thighElement);
                     }

                     if (item.partName == "calf")
                     {
                         if (calf >2)
                         {
                             QMessageBox::information(0, QString("OOPS"),
                                            QString("Error reason: \n2. Three claf? Do you need some COFFEE?Please LOAD this image AGAIN."),
                                            QMessageBox::Ok);
                             groupAnnotationRecord.clear ();
                             scene_ID.clear ();
                             controlPanel->hide ();
                             return;
                         }


                         element = doc.createElement (limbs[LRC]);
                         LRC++;
                         element.appendChild (centerNode);
                         element.appendChild (widthNode);
                         element.appendChild (heightNode);
                         element.appendChild (rotationNode);
                         calfElement.appendChild (element);
                         target.appendChild (calfElement);
                     }

                 }


             }

             scene.appendChild (target);


        }
        doc.appendChild (scene);


        QTextStream out(&resultFile);
        out.setCodec("UTF-8");
        doc.save (out,4, QDomNode::EncodingFromTextStream);

        scene_ID.clear ();
        groupAnnotationRecord.clear ();
        controlPanel->hide ();

        QMessageBox::information(0, QString("notification"),
                                 QString("Great job! Result is stored in file.\nYou can drag another image."),
                                 QMessageBox::Ok);

    }
    else
    {
        QMessageBox::information(0, QString("OOPS"),
                                 QString("You haven't annotated yet or you have already saved once. \
                                         Please annotate or drop another image."),
                                 QMessageBox::Ok);

    }

}

void MainWindow::on_labelMe_clicked()
{
    if (beginAnnotation && group)
    {
        pen = QPen(Qt::white);
        pen.setWidth (3);
        annotatingUse.partName = "location";
    }
}
