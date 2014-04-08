#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QFileSystemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMainWindow>
#include <QSettings>

namespace Ui {
class FileWindow;
}

class FileWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileWindow(QWidget *parent = 0);
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

    void on_CBMStatus_clicked();

    void on_actionView_Drive_triggered();

    void on_actionView_Home_Folder_triggered();

private:
    Ui::FileWindow *ui;
    QFileSystemModel *foldersModel;
    QFileSystemModel *filesModel;
    QItemSelectionModel *selectModel;
    QSettings *settings;
    QString formatFileSize(qint64);
    QString cbmctrl;
    QString cbmforng;
    QString d64copy;
    QString transfermode;
    bool showcmd;
    bool autorefresh;
    int deviceid;
};

#endif // FILEWINDOW_H
