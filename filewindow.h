#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QFileSystemModel>
#include <QFontDatabase>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QTimer>

namespace Ui {
class FileWindow;
}

class FileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileWindow(QWidget *parent = 0);
    bool confirmExecute(QString command, QStringList params);
    ~FileWindow();  

private slots:
    void on_localFolders_clicked(const QModelIndex &index);

    void localFiles_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void act_newFolder();

    void act_renameFile();

    void act_deleteFile();

    void act_viewFile();

    void on_localFiles_doubleClicked(const QModelIndex &index);

    void on_actionPreferences_triggered();

    void loadSettings();
    void cbmStatusFinished(int,QProcess::ExitStatus);
    void cbmCopyProgress();
    void cbmCopyFinished(int,QProcess::ExitStatus);
    void cbmDirFinished(int, QProcess::ExitStatus);
    void cbmResetFinished(int, QProcess::ExitStatus);
    void cbmFormatFinished(int, QProcess::ExitStatus);
    void cbmInitFinished(int, QProcess::ExitStatus);
    void cbmValidateFinished(int, QProcess::ExitStatus);
    void cbmScratchFinished(int ,QProcess::ExitStatus);
    void cbmRenameFinished(int ,QProcess::ExitStatus);
    void stopCopy();
    void timerClick();

    void on_CBMStatus_clicked();

    void on_actionView_Drive_triggered();

    void on_actionView_Home_Folder_triggered();

    void on_copyToCBM_clicked();

    void on_CBMDirectory_clicked();

    void on_actionAbout_triggered();

    void on_CBMReset_clicked();

    void on_CBMFormat_clicked();

    void on_CBMInitialize_clicked();

    void on_CBMValidate_clicked();

    void on_copyFromCBM_clicked();

    void on_CBMScratch_clicked();

    void on_CBMRename_clicked();

private:
    Ui::FileWindow *ui;
    QFileSystemModel *foldersModel;
    QFileSystemModel *filesModel;
    QFontDatabase *fontDB;
    QItemSelectionModel *selectModel;
    QSettings *settings;
    QProcess *proc_cbmStatus;
    QProcess *proc_d64copy;
    QProcess *proc_cbmDir;
    QProcess *proc_cbmReset;
    QProcess *proc_cbmFormat;
    QProcess *proc_cbmInit;
    QProcess *proc_cbmValidate;
    QProcess *proc_cbmScratch;
    QProcess *proc_cbmRename;
    QProgressBar *progbar;
    QPushButton *btn_abort;
    QString formatFileSize(qint64);
    QString cbmctrl;
    QString cbmforng;
    QString d64copy;
    QString transfermode;
    QString d64imageFile;
    QTimer *timer;
    bool showcmd;
    bool autorefresh;
    bool usec64font;
    int deviceid;
    int lastBlock;
    int currBlock;
};

#endif // FILEWINDOW_H
