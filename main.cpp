#include "filewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileWindow w;
    w.show();

    return a.exec();
}
