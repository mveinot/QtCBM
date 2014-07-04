#include "filewindow.h"
#include <QApplication>
#include <QtDebug>
#include <QtSingleApplication>
#include <QMessageBox>
#include <QStandardPaths>

void localMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txt;

    switch (type)
    {
        case QtDebugMsg:
            txt = QString("DBUG: %1").arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("WARN: %1 in %2 at line %3").arg(msg).arg(context.file).arg(context.line);
            break;
        case QtCriticalMsg:
            txt = QString("CRIT: %1 in %2 at line %3").arg(msg).arg(context.file).arg(context.line);
            break;
        case QtFatalMsg:
            txt = QString("FATL: %1 in %2 at line %3").arg(msg).arg(context.file).arg(context.line);
            break;
        default:
            txt = QString("Undefined message type. Message: %1").arg(msg);
    }

    QString directory = QStandardPaths::locate(QStandardPaths::DataLocation, QString(), QStandardPaths::LocateDirectory);
    if (directory.isEmpty())
        directory = QDir::homePath() + "/." + QCoreApplication::applicationName();

    //QMessageBox::information(0, "directory", directory, QMessageBox::Ok, QMessageBox::Ok);

    if (!QFile::exists(directory))
    {
        QDir dir;
        dir.mkpath(directory);
    }

    QFile outFile(directory + "/messages.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    ts.flush();
    outFile.close();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(localMessageHandler);
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
