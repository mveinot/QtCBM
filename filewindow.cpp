#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfoList>
#include <QFileSystemModel>
#include <QFontDatabase>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegExp>
#include <QString>
#include <QTreeWidgetItem>
#include <QUrl>

#include <QtDebug>

#include "filewindow.h"
#include "ui_filewindow.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "detailsinfodialog.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

FileWindow::FileWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileWindow)
{
    QAction *actMakeDir, *actRenameFile, *actDeleteFile, *actViewFile;

    ui->setupUi(this);

    // Load the C64 system font from resources
    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/res/fonts/c64.ttf");
    QFont font8("C64 Pro Mono", 8, -1, false);
    QFont font11("C64 Pro Mono", 12, -1, false);

    QString c64TreeStyle = "QTreeWidget {background-color: #4E2EDE; color: #A7A1FD; }";
    QString c64LineStyle = "QLineEdit {background-color: #4E2EDE; color: #A7A1FD; }";

    // connect the signals for handling out of thread execution
    proc_cbmStatus = new QProcess(this);
    connect(proc_cbmStatus, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cbmStatusFinished(int,QProcess::ExitStatus)));

    proc_d64copy = new QProcess(this);
    connect(proc_d64copy, SIGNAL(readyReadStandardOutput()), this, SLOT(cbmCopyProgress()));
    connect(proc_d64copy, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmCopyFinished(int,QProcess::ExitStatus)));

    proc_cbmDir = new QProcess(this);
    connect(proc_cbmDir, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmDirFinished(int,QProcess::ExitStatus)));

    proc_cbmReset = new QProcess(this);
    connect(proc_cbmReset, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmResetFinished(int,QProcess::ExitStatus)));

    proc_cbmFormat = new QProcess(this);
    connect(proc_cbmFormat, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmFormatFinished(int,QProcess::ExitStatus)));

    proc_cbmInit = new QProcess(this);
    connect(proc_cbmInit, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmInitFinished(int,QProcess::ExitStatus)));

    proc_cbmValidate = new QProcess(this);
    connect(proc_cbmValidate, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmValidateFinished(int,QProcess::ExitStatus)));

    proc_cbmScratch = new QProcess(this);
    connect(proc_cbmScratch, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmScratchFinished(int,QProcess::ExitStatus)));

    proc_cbmRename = new QProcess(this);
    connect(proc_cbmRename, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmRenameFinished(int,QProcess::ExitStatus)));

    // initialize the settings object
    settings = new QSettings("mvgrafx", "QtCBM");
    loadSettings();

    // Create our context menu items
    actMakeDir = new QAction(tr("&New Folder..."),this);
    connect(actMakeDir, SIGNAL(triggered()), this, SLOT(act_newFolder()));
    ui->localFolders->addAction(actMakeDir);
    ui->localFolders->setContextMenuPolicy(Qt::ActionsContextMenu);

    actRenameFile = new QAction(tr("&Rename..."), this);
    actDeleteFile = new QAction(tr("&Delete..."),this);
    actViewFile = new QAction(tr("&Run/View"),this);

    connect(actRenameFile, SIGNAL(triggered()), this, SLOT(act_renameFile()));
    connect(actDeleteFile, SIGNAL(triggered()), this, SLOT(act_deleteFile()));
    connect(actViewFile, SIGNAL(triggered()), this, SLOT(act_viewFile()));

    ui->localFiles->addAction(actRenameFile);
    ui->localFiles->addAction(actDeleteFile);
    ui->localFiles->addAction(actViewFile);

    ui->localFiles->setContextMenuPolicy(Qt::ActionsContextMenu);

    // set up the folders view
    foldersModel = new QFileSystemModel(this);
    foldersModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    foldersModel->setRootPath(QDir::rootPath());
    ui->localFolders->setModel(foldersModel);
    ui->localFolders->setRootIndex(foldersModel->index(QDir::homePath()));
    ui->localFolders->hideColumn(1);
    ui->localFolders->hideColumn(2);
    ui->localFolders->hideColumn(3);
    ui->localFolders->setAnimated(false);

    // size the CBM file list headers
    ui->cbmFiles->header()->resizeSection(0, 45);
    ui->cbmFiles->header()->resizeSection(1, 60);
    ui->cbmFiles->header()->resizeSection(2, 140);
    ui->cbmFiles->header()->resizeSection(3, 40);

    // set the colors of the CBM elements
    ui->diskLabel->setStyleSheet(c64LineStyle);
    ui->diskId->setStyleSheet(c64LineStyle);
    ui->freeSpace->setStyleSheet(c64LineStyle);
    ui->cbmFiles->setStyleSheet(c64TreeStyle);
}

void FileWindow::loadSettings()
{
    QFont font11;
    QFont font8;
    // read in settings
    cbmctrl = settings->value("tools/cbmctrl", QStandardPaths::findExecutable("cbmctrl.exe")).toString();
    cbmforng = settings->value("tools/cbmforng", QStandardPaths::findExecutable("cbmforng.exe")).toString();
    d64copy = settings->value("tools/d64copy", QStandardPaths::findExecutable("d64copy.exe")).toString();
    deviceid = settings->value("deviceid", 8).toInt();
    transfermode = settings->value("transfermode", "auto").toString();
    showcmd = settings->value("showcmd", false).toBool();
    autorefresh = settings->value("autorefresh", true).toBool();
    usec64font = settings->value("usec64font", false).toBool();

    // Load the C64 system font from resources
    fontDB = new QFontDatabase();
    if (usec64font)
    {
        fontDB->addApplicationFont(":/res/fonts/c64.ttf");
        font8 = QFont("C64 Pro Mono", 8, -1, false);
        font11 = QFont("C64 Pro Mono", 12, -1, false);
    } else
    {
        fontDB->addApplicationFont(":/res/fonts/Consolas.ttf");
        font8 = QFont("Consolas", 14, -1, false);
        font11 = QFont("Consolas", 14, -1, false);
    }

    ui->diskLabel->setFont(font11);
    ui->diskId->setFont(font11);
    ui->freeSpace->setFont(font11);
    ui->cbmFiles->setFont(font8);

    ui->statusBar->showMessage("Settings read", 5000);
    //QMessageBox::information(this, "settings", cbmctrl+cbmforng+d64copy, QMessageBox::Ok, QMessageBox::Ok);
}

FileWindow::~FileWindow()
{
    delete ui;
}

void FileWindow::on_localFolders_clicked(const QModelIndex &index)
{
    QFileSystemModel *model = (QFileSystemModel*)ui->localFolders->model();
    QDir *dir = new QDir(model->filePath(index));

    filesModel = new QFileSystemModel();
    filesModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    filesModel->setRootPath(dir->path());
    ui->localFiles->setModel(filesModel);
    ui->localFiles->setRootIndex(filesModel->index(dir->path()));
    ui->localFiles->setCurrentIndex(filesModel->index(0,0,ui->localFiles->rootIndex()));
    ui->localFiles->setUniformItemSizes(true);

    selectModel = ui->localFiles->selectionModel();
    connect(selectModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(localFiles_selectionChanged(QItemSelection,QItemSelection)));
}

void FileWindow::localFiles_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    (void)selected;
    (void)deselected;

    qint64 size, blocks;
    size = blocks = 0;

    QModelIndexList list = ui->localFiles->selectionModel()->selectedIndexes();

    foreach (const QModelIndex &index, list)
    {
        QFileSystemModel *model = (QFileSystemModel*)ui->localFiles->model();
        QFile *file = new QFile(model->filePath(index));
        size+=file->size();
    }
    blocks = size / 254;
    QString sizeString = formatFileSize(size);
    QString blockString = QString::number(blocks);
    ui->selectedKB->setText(sizeString);
    ui->selectedBlocks->setText(blockString.append(" blocks"));
}

void FileWindow::act_newFolder()
{
    bool ok;

    QModelIndexList index = ui->localFolders->selectionModel()->selectedIndexes();
    QFileSystemModel *model = (QFileSystemModel*)ui->localFolders->model();
    QDir *dir = new QDir(model->filePath(index.at(0)));

    QString text = QInputDialog::getText(this, tr("New Folder"), tr("Folder name:"), QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty())
    {
        if (dir->mkdir(text))
        {
            ui->statusBar->showMessage(tr("Folder created"), 5000);
        } else
        {
            QMessageBox::warning(this,tr("Error"),tr("Could not create a new folder"), QMessageBox::Ok,QMessageBox::Ok);
        }
    }
}

void FileWindow::act_renameFile()
{
    bool ok;

    QModelIndexList index = ui->localFiles->selectionModel()->selectedIndexes();
    QFileSystemModel *model = (QFileSystemModel*)ui->localFiles->model();

    if (index.count() < 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("No files selected"), QMessageBox::Ok, QMessageBox::Ok);
    } else if (index.count() > 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("Can't rename multiple files"), QMessageBox::Ok, QMessageBox::Ok);
    } else {
        QFile *file = new QFile(model->filePath(index.at(0)));
        QFileInfo fileInfo(model->filePath(index.at(0)));

        QString text = QInputDialog::getText(this, tr("Rename File"), tr("New name:"), QLineEdit::Normal, fileInfo.baseName()+"."+fileInfo.completeSuffix(), &ok);
        if (ok && !text.isEmpty())
        {
            QString newPath = fileInfo.canonicalPath()+QDir::separator()+text;
            if (file->rename(newPath))
            {
                ui->statusBar->showMessage(tr("File renamed"), 5000);
            } else
            {
                QMessageBox::warning(this,tr("Error"),tr("Could not rename file"), QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }
}

void FileWindow::act_deleteFile()
{
    QModelIndexList index = ui->localFiles->selectionModel()->selectedIndexes();
    QFileSystemModel *model = (QFileSystemModel*)ui->localFiles->model();

    if (index.count() < 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("No files selected"), QMessageBox::Ok, QMessageBox::Ok);
    } else if (index.count() > 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("Won't delete multiple files"), QMessageBox::Ok, QMessageBox::Ok);
    } else {
        QFile *file = new QFile(model->filePath(index.at(0)));

        if (QMessageBox::question(this,tr("Delete File?"), tr("Are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            if (file->remove())
            {
                ui->statusBar->showMessage(tr("File deleted"), 5000);
            } else
            {
                QMessageBox::warning(this,tr("Error"),tr("Could not delete file"), QMessageBox::Ok, QMessageBox::Ok);
            }
        }
    }
}

void FileWindow::act_viewFile()
{
    QModelIndexList index = ui->localFiles->selectionModel()->selectedIndexes();
    QFileSystemModel *model = (QFileSystemModel*)ui->localFiles->model();

    if (index.count() < 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("No files selected"), QMessageBox::Ok, QMessageBox::Ok);
    } else if (index.count() > 1)
    {
        QMessageBox::warning(this,tr("Error"), tr("Can't run multiple files"), QMessageBox::Ok, QMessageBox::Ok);
    } else {
        QDesktopServices::openUrl(QUrl::fromLocalFile(model->filePath(index.at(0))));
    }
}

QString FileWindow::formatFileSize(qint64 size)
{
    float outputsize = 0;

    if (size > (1024*1024*1024))
    {
        outputsize = size / (1024*1024*1024);
        return QString::number(outputsize).append(" GB");
    }
    else if (size > (1024*1024))
    {
        outputsize = size / (1024*1024);
        return QString::number(outputsize).append(" MB");
    }
    else if (size > (1024))
    {
        outputsize = size / (1024);
        return QString::number(outputsize).append(" KB");
    }
    else
    {
        outputsize = size;
        return QString::number(outputsize).append(" B");
    }
}

void FileWindow::on_localFiles_doubleClicked(const QModelIndex &index)
{
    (void)index;
    act_viewFile();
}

void FileWindow::on_actionPreferences_triggered()
{
    settingsDialog *dlg = new settingsDialog();

    connect(dlg, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));

    dlg->show();
}

void FileWindow::on_CBMStatus_clicked()
{
    if (confirmExecute(cbmctrl, QStringList() << "status" << QString::number(deviceid)))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmStatus->start(cbmctrl, QStringList() << "status" << QString::number(deviceid), QIODevice::ReadWrite | QIODevice::Text);
        if (!proc_cbmStatus->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmStatus->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}

void FileWindow::on_actionView_Drive_triggered()
{
    ui->localFolders->setRootIndex(foldersModel->index(QDir::rootPath()));
    ui->actionView_Home_Folder->setChecked(false);
    ui->actionView_Drive->setChecked(true);
}

void FileWindow::on_actionView_Home_Folder_triggered()
{
    ui->localFolders->setRootIndex(foldersModel->index(QDir::homePath()));
    ui->actionView_Home_Folder->setChecked(true);
    ui->actionView_Drive->setChecked(false);
}

void FileWindow::cbmStatusFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;
    ui->statusBar->showMessage("Drive status: "+proc_cbmStatus->readAllStandardOutput());
}

void FileWindow::on_copyToCBM_clicked()
{
    QString fileToCopy;

    if (ui->localFiles->model() != NULL)
    {
        QModelIndexList index = ui->localFiles->selectionModel()->selectedIndexes();
        QFileSystemModel *model = (QFileSystemModel*)ui->localFiles->model();

        if (index.count() < 1)
        {
            QMessageBox::warning(this,tr("Error"), tr("No files selected"), QMessageBox::Ok, QMessageBox::Ok);
            return;
        } else if (index.count() > 1)
        {
            QMessageBox::warning(this,tr("Error"), tr("Can't image multiple files"), QMessageBox::Ok, QMessageBox::Ok);
            return;
        } else {
            fileToCopy = model->filePath(index.at(0));
        }
    } else
    {
        QMessageBox::warning(this,tr("Error"), tr("No files selected"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (confirmExecute(d64copy, QStringList() << "--transfer="+transfermode << QDir::toNativeSeparators(fileToCopy) << QString::number(deviceid)))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(100);
        progbar->setTextVisible(true);
        btn_abort = new QPushButton(this);
        connect(btn_abort, SIGNAL(clicked()), this, SLOT(stopCopy()));
        btn_abort->setText("X");
        btn_abort->setToolTip("Abort the current transfer and reset the CBM bus");
        btn_abort->setFixedHeight(18);
        btn_abort->setFixedWidth(18);
        ui->statusBar->addPermanentWidget(progbar);
        ui->statusBar->addPermanentWidget(btn_abort);
        timer = new QTimer(this);
        timer->setInterval(10000);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerClick()));
        ui->copyToCBM->setEnabled(false);
        QFileInfo file(fileToCopy);
        ui->statusBar->showMessage("Writing: "+file.baseName()+"."+file.completeSuffix()+"...");
        d64imageFile = file.baseName()+"."+file.completeSuffix();

        proc_d64copy->start(d64copy, QStringList() << "--transfer="+transfermode << QDir::toNativeSeparators(fileToCopy) << QString::number(deviceid), QIODevice::ReadWrite | QIODevice::Text);
        if (!proc_d64copy->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+d64copy+"\n\nExit status: "+QString::number(proc_d64copy->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            ui->statusBar->removeWidget(btn_abort);
            delete progbar;
            delete btn_abort;
        }
        timer->start();
        currBlock = 0;
        lastBlock = 0;
    }
}

void FileWindow::timerClick()
{
    if (currBlock > lastBlock)
    {
        //qDebug() << "copy in progress" << currBlock;
        lastBlock = currBlock;
    } else
    {
        //qDebug() << "copy might be stuck";
        if (QMessageBox::critical(this, "QtCBM", "Trouble writing "+d64imageFile+"\n\nAbort?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
        {
            stopCopy();
        } else {
            lastBlock = currBlock;
        }
    }
}

void FileWindow::stopCopy()
{
    timer->stop();
    //qDebug() << "requested abort copy operation";
    proc_d64copy->kill();
    //ui->statusBar->removeWidget(btn_abort);
    //ui->statusBar->removeWidget(progbar);
    //delete btn_abort;
    //delete progbar;
    //ui->statusBar->showMessage(proc_d64copy->readAllStandardOutput());
    ui->copyToCBM->setEnabled(true);
    QMessageBox::information(this, "Copy Aborted", "The copy operation was terminated", QMessageBox::Ok, QMessageBox::Ok);
}

void FileWindow::cbmCopyFinished(int x, QProcess::ExitStatus status)
{
    timer->stop();
    qDebug() << "Process terminated with exit code: "+QString::number(x)+" and exit status: "+QString::number(status);
    (void)x;
    (void)status;

    ui->statusBar->removeWidget(btn_abort);
    ui->statusBar->removeWidget(progbar);
    delete btn_abort;
    delete progbar;
    ui->copyToCBM->setEnabled(true);
    if (status == QProcess::NormalExit)
    {
        on_CBMDirectory_clicked();
    } else if (status == QProcess::CrashExit)
    {
        ui->statusBar->clearMessage();
#ifdef Q_OS_WIN
        Sleep(2);
#endif
        on_CBMReset_clicked();
    }
}

void FileWindow::cbmCopyProgress()
{
    QString output = proc_d64copy->readAllStandardOutput();
    QRegExp rx("\\s?(\\d+):\\s*([\\*\\.\\-\\?]+)\\s*(\\d+)%\\s*(\\d+)\\/(\\d+).*");
    QRegExp rxTrackChange("\\s?(\\d+):\\s*([\\*\\.\\-\\?]+)\\s*");
    QRegExp rxDone(".*(\\d+ blocks copied)\\.");
    if (rx.indexIn(output) >= 0)
    {
        progbar->setValue(rx.cap(3).toInt());
        currBlock = rx.cap(4).toInt();
#ifdef Q_OS_WIN
        progbar->setFormat("Track: "+rx.cap(1)+" Block: "+rx.cap(4)+"/"+rx.cap(5));
#endif
#ifdef Q_OS_MAC
        ui->statusBar->showMessage("Track: "+rx.cap(1)+", Block: "+rx.cap(4)+"/"+rx.cap(5));
#endif
    } else if (rxTrackChange.indexIn(output) >= 0)
    {
        //qDebug() << "Next track detected";
    } else if (rxDone.indexIn(output) >= 0)
    {
        ui->statusBar->showMessage(rxDone.cap(1));
    }
}

void FileWindow::cbmResetFinished(int,QProcess::ExitStatus)
{
    // remove the progress bar
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    ui->statusBar->showMessage("The CBM bus was reset");
}

void FileWindow::cbmInitFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    ui->statusBar->showMessage("Initialization complete");
}

void FileWindow::cbmValidateFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    ui->statusBar->showMessage("Validation complete");
}

void FileWindow::cbmFormatFinished(int, QProcess::ExitStatus)
{
    QString output = proc_cbmFormat->readAllStandardOutput();
    //qDebug() << output;

    ui->statusBar->removeWidget(progbar);
    delete progbar;

    detailsInfoDialog *dlg = new detailsInfoDialog(this);
    dlg->setText("The format command produced the following output:");
    dlg->setDetailText(output);
    dlg->exec();
}

void FileWindow::cbmDirFinished(int, QProcess::ExitStatus)
{
    // remove the progress bar
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    // clear the file list
    ui->cbmFiles->clear();

    QList<QByteArray> dirlist = proc_cbmDir->readAllStandardOutput().split('\n');
    for (int i = 0; i < dirlist.count(); i++)
    {
        QRegExp rxLabel("(0|1)\\s*.\"(.*)\"\\s*(.*)");
        QRegExp rxDirEntry("(\\d+)\\s*\"(.*)\"\\s+(\\S\\S\\S)");
        QRegExp rxFreeSpace("(\\d+) blocks free.");
        if (rxDirEntry.indexIn(QString(dirlist.at(i))) >= 0)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, rxDirEntry.cap(1).toUpper());
            item->setText(1, formatFileSize(rxDirEntry.cap(1).toInt()*254));
            item->setText(2, rxDirEntry.cap(2).toUpper());
            item->setText(3, rxDirEntry.cap(3).toUpper());
            ui->cbmFiles->addTopLevelItem(item);
        } else if (rxFreeSpace.indexIn(QString(dirlist.at(i))) >= 0)
        {
            ui->freeSpace->setText(rxFreeSpace.cap(1)+" blocks ("+formatFileSize(rxFreeSpace.cap(1).toInt()*254)+") free");
        } else if (rxLabel.indexIn(QString(dirlist.at(i))) >= 0)
        {
            ui->diskLabel->setText(rxLabel.cap(2).trimmed());
            ui->diskId->setText(rxLabel.cap(3));
        }
        //qDebug() << QString(dirlist.at(i));
    }
}

bool FileWindow::confirmExecute(QString command, QStringList params)
{
    if (showcmd)
    {
        QFileInfo file = QFileInfo(command);
        return (QMessageBox::information(this, "QtCBM", "About to execute:\n\n"+file.baseName()+"."+file.completeSuffix()+" "+params.join(' ')+"\n\nContinue?", QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes);
    }
    return true;
}

void FileWindow::on_CBMDirectory_clicked()
{
    if(confirmExecute(cbmctrl, QStringList() << "dir" << QString::number(deviceid)))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmDir->start(cbmctrl, QStringList() << "dir" << QString::number(deviceid), QIODevice::ReadWrite | QIODevice::Text);
        if (!proc_cbmDir->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmDir->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}

void FileWindow::on_actionAbout_triggered()
{
    aboutDialog *dlg = new aboutDialog(this);
    dlg->show();
}

void FileWindow::on_CBMReset_clicked()
{
    if (confirmExecute(cbmctrl, QStringList() << "reset"))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmReset->start(cbmctrl, QStringList() << "reset", QIODevice::ReadWrite | QIODevice::Text);
        if (!proc_cbmReset->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmReset->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}

void FileWindow::on_CBMFormat_clicked()
{
    QString diskLabel = "";
    bool ok;

    if (QMessageBox::question(this, "QtCBM", "This will erase ALL data on the floppy disk. Continue?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {
        diskLabel = QInputDialog::getText(this, "QtCBM", "Diskname,ID:", QLineEdit::Normal, "", &ok).toUpper();
        QRegExp rx("[\\s\\S]+,\\d+");
        if (rx.indexIn(diskLabel) >= 0)
        {
        if (ok && confirmExecute(cbmforng, QStringList() << QString::number(deviceid) << diskLabel))
        {
            progbar = new QProgressBar(this);
            progbar->setMinimum(0);
            progbar->setMaximum(0);
            ui->statusBar->addPermanentWidget(progbar);

            proc_cbmFormat->start(cbmforng, QStringList() << QString::number(deviceid) << diskLabel);
            if (!proc_cbmFormat->waitForStarted())
            {
                QMessageBox::warning(this,"Error", "Failed to execute "+cbmforng+"\n\nExit status: "+QString::number(proc_cbmFormat->exitCode()), QMessageBox::Ok, QMessageBox::Ok);
                ui->statusBar->removeWidget(progbar);
                delete progbar;
            }
        }
        } else
        {
            QMessageBox::warning(this, "QtCBM", "Your input wasn't of the form \"Label,ID\". Unable to format with this input.");
        }
    }
}

void FileWindow::on_CBMInitialize_clicked()
{
    if (confirmExecute(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "IO"))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmInit->start(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "IO");
        if (!proc_cbmInit->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmInit->exitCode()), QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}

void FileWindow::on_CBMValidate_clicked()
{
    if (confirmExecute(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "V0:"))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmValidate->start(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "V0:");
        if (!proc_cbmValidate->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmValidate->exitCode()), QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}

void FileWindow::on_copyFromCBM_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Disk Image"), QDir::homePath(), tr("Disk Images (*.d64)"));

    if (!fileName.isEmpty() && confirmExecute(d64copy, QStringList() << "--transfer="+transfermode << QString::number(deviceid) << QDir::toNativeSeparators(fileName)))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(100);
        progbar->setTextVisible(true);
        btn_abort = new QPushButton(this);
        connect(btn_abort, SIGNAL(clicked()), this, SLOT(stopCopy()));
        btn_abort->setText("X");
        btn_abort->setToolTip("Abort the current transfer and reset the CBM bus");
        btn_abort->setFixedHeight(18);
        btn_abort->setFixedWidth(18);
        ui->statusBar->addPermanentWidget(progbar);
        ui->statusBar->addPermanentWidget(btn_abort);
        timer = new QTimer(this);
        timer->setInterval(10000);
        connect(timer, SIGNAL(timeout()), this, SLOT(timerClick()));
        ui->copyToCBM->setEnabled(false);
        QFileInfo file(fileName);
        ui->statusBar->showMessage("Writing: "+file.baseName()+"."+file.completeSuffix()+"...");
        d64imageFile = file.baseName()+"."+file.completeSuffix();

        proc_d64copy->start(d64copy, QStringList() << "--transfer="+transfermode << QString::number(deviceid) << QDir::toNativeSeparators(fileName), QIODevice::ReadWrite | QIODevice::Text);
        if (!proc_d64copy->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+d64copy+"\n\nExit status: "+QString::number(proc_d64copy->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            ui->statusBar->removeWidget(btn_abort);
            delete progbar;
            delete btn_abort;
        }
        timer->start();
        currBlock = 0;
        lastBlock = 0;
    }
}

void FileWindow::cbmRenameFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    ui->statusBar->showMessage("File renamed");
    on_CBMDirectory_clicked();
}

void FileWindow::cbmScratchFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;

    ui->statusBar->showMessage("File erased");
    on_CBMDirectory_clicked();
}

void FileWindow::on_CBMScratch_clicked()
{
    QList<QTreeWidgetItem *> items = ui->cbmFiles->selectedItems();
    if (items.count() != 1)
    {
        QMessageBox::warning(this, "QtCBM", "Please select one item to delete.", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString cbmFilename = items.at(0)->data(2,0).toString();

    if (QMessageBox::question(this, "QtCBM", "Are you sure you want to delete the file \""+cbmFilename+"\" from the current disk?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes)
    {

        if (confirmExecute(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "S0:"+cbmFilename))
        {
            progbar = new QProgressBar(this);
            progbar->setMinimum(0);
            progbar->setMaximum(0);
            ui->statusBar->addPermanentWidget(progbar);

            proc_cbmScratch->start(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "S0:"+cbmFilename);
            if (!proc_cbmScratch->waitForStarted())
            {
                QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmScratch->exitCode()), QMessageBox::Ok, QMessageBox::Ok);
                ui->statusBar->removeWidget(progbar);
                delete progbar;
            }
        }
    }
}

void FileWindow::on_CBMRename_clicked()
{
    bool ok = false;
    QList<QTreeWidgetItem *> items = ui->cbmFiles->selectedItems();
    if (items.count() != 1)
    {
        QMessageBox::warning(this, "QtCBM", "Please select a file to rename.", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    QString cbmFilename = items.at(0)->data(2,0).toString();

    QString newFilename = QInputDialog::getText(this, "QtCBM", "New filename:", QLineEdit::Normal, "", &ok);
    if ( ok && confirmExecute(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "R0:"+cbmFilename+"="+newFilename))
    {
        progbar = new QProgressBar(this);
        progbar->setMinimum(0);
        progbar->setMaximum(0);
        ui->statusBar->addPermanentWidget(progbar);

        proc_cbmRename->start(cbmctrl, QStringList() << "command" << QString::number(deviceid) << "R0:"+cbmFilename+"="+newFilename);
        if (!proc_cbmRename->waitForStarted())
        {
            QMessageBox::warning(this,"Error", "Failed to execute "+cbmctrl+"\n\nExit status: "+QString::number(proc_cbmRename->exitCode()), QMessageBox::Ok, QMessageBox::Ok);
            ui->statusBar->removeWidget(progbar);
            delete progbar;
        }
    }
}
