#include "widget.h"
#include <QApplication>
#include <QtWebEngineWidgets>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    widget w;

    w.setWindowTitle(QString::fromLocal8Bit("基于北斗短报文的无人机导航系统"));
    w.show();

    return a.exec();
}
