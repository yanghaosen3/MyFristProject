#include "ScreenCaptureWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ScreenCaptureWidget w;
    w.show();
    return a.exec();
}
