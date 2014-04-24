#include "filewindow.h"
#include <QApplication>
#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileWindow w;
    w.show();
    if (argc > 1)
        w.writeD64FromArgs(QString(argv[1]));

    return a.exec();
}
