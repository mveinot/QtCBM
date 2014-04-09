#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QFileSystemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QRegExp>
#include <QString>
#include <QUrl>

#include <QtDebug>

#include "filewindow.h"
#include "ui_filewindow.h"
#include "settingsdialog.h"

FileWindow::FileWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileWindow)
{
    QAction *actMakeDir, *actRenameFile, *actDeleteFile, *actViewFile;

    ui->setupUi(this);

    proc_cbmStatus = new QProcess(this);
    connect(proc_cbmStatus, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(cbmStatusFinished(int,QProcess::ExitStatus)));

    proc_d64copy = new QProcess(this);
    connect(proc_d64copy, SIGNAL(readyReadStandardOutput()), this, SLOT(cbmCopyProgress()));
    connect(proc_d64copy, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmCopyFinished(int,QProcess::ExitStatus)));

    proc_cbmDir = new QProcess(this);
    connect(proc_cbmDir, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(cbmDirFinished(int,QProcess::ExitStatus)));

    settings = new QSettings("mvgrafx", "QtCBM");
    loadSettings();

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

    foldersModel = new QFileSystemModel(this);
    foldersModel->setFilter(QDir::NoDotAndDotDot | QDir::Dirs);
    foldersModel->setRootPath(QDir::rootPath());
    ui->localFolders->setModel(foldersModel);
    ui->localFolders->setRootIndex(foldersModel->index(QDir::homePath()));
    ui->localFolders->hideColumn(1);
    ui->localFolders->hideColumn(2);
    ui->localFolders->hideColumn(3);
    ui->localFolders->setAnimated(false);

    ui->cbmFiles->header()->resizeSection(0, 50);
    ui->cbmFiles->header()->resizeSection(1, 60);
    ui->cbmFiles->header()->resizeSection(2, 150);
}

void FileWindow::loadSettings()
{
    cbmctrl = settings->value("tools/cbmctrl", QStandardPaths::findExecutable("cbmctrl.exe")).toString();
    cbmforng = settings->value("tools/cbmforng", QStandardPaths::findExecutable("cbmforng.exe")).toString();
    d64copy = settings->value("tools/d64copy", QStandardPaths::findExecutable("d64copy.exe")).toString();
    deviceid = settings->value("deviceid", 8).toInt();
    transfermode = settings->value("transfermode", "auto").toString();
    showcmd = settings->value("showcmd", false).toBool();
    autorefresh = settings->value("autorefresh", true).toBool();
    ui->statusBar->showMessage("Settings read", 5000);
    //qDebug() << cbmctrl << cbmforng << d64copy << deviceid << transfermode << showcmd << autorefresh;
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

    progbar = new QProgressBar(this);
    progbar->setMinimum(0);
    progbar->setMaximum(100);
    ui->statusBar->addPermanentWidget(progbar);
    ui->copyToCBM->setEnabled(false);

    proc_d64copy->start(d64copy, QStringList() << fileToCopy << QString::number(deviceid), QIODevice::ReadWrite | QIODevice::Text);
    if (!proc_d64copy->waitForStarted())
    {
        QMessageBox::warning(this,"Error", "Failed to execute "+d64copy+"\n\nExit status: "+QString::number(proc_d64copy->exitCode()),QMessageBox::Ok, QMessageBox::Ok);
	ui->statusBar->removeWidget(progbar);
	delete progbar;
    }
}

void FileWindow::cbmCopyFinished(int, QProcess::ExitStatus)
{
    ui->statusBar->removeWidget(progbar);
    delete progbar;
    ui->statusBar->showMessage(proc_d64copy->readAllStandardOutput());
    ui->copyToCBM->setEnabled(true);
    on_CBMDirectory_clicked();
}

void FileWindow::cbmCopyProgress()
{
    QRegExp rx("\\s?(\\d+):\\s*\\*+\\s*(\\d+)%\\s*(\\d+)\\/(\\d+).*");
    if (rx.indexIn(proc_d64copy->readAllStandardOutput()) >= 0)
    {
        ui->statusBar->showMessage("Track: "+rx.cap(1)+" Block: "+rx.cap(3)+"/"+rx.cap(4));
        progbar->setValue(rx.cap(2).toInt());
    } else
    {
        ui->statusBar->showMessage(proc_d64copy->readAllStandardOutput());
    }
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
            item->setText(0, rxDirEntry.cap(1));
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

void FileWindow::on_CBMDirectory_clicked()
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
