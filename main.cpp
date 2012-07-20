#include <QtGui/QApplication>
#include "mainwindow.h"
#include "controlform.h"
#include "globalvar.h"


int main(int argc, char *argv[])
{
    QApplication::addLibraryPath("plugins");
    QApplication a(argc, argv);
    MainWindow annotationToolMainWindow;
    annotationToolMainWindow.setWindowTitle ("Manual Annotation Tool");

    annotationToolMainWindow.show();


    return a.exec();
}
