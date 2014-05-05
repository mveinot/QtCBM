#include "filewindow.h"
#include <QApplication>
#include <QtSingleApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QtSingleApplication a("QtCBM", argc, argv);

    if (a.isRunning())
    {
        a.sendMessage("Already started");
        QMessageBox::warning(NULL, "QtCBM", "Another instance of this application is already running. Press OK to exit.", QMessageBox::Ok, QMessageBox::Ok);
        return 1;
    }

    FileWindow w;
    w.show();

    a.setActivationWindow(&w);

    if (argc > 1)
        w.writeD64FromArgs(QString(argv[1]));

    return a.exec();
}
