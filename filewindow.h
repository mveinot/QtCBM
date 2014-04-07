#ifndef FILEWINDOW_H
#define FILEWINDOW_H

#include <QFileSystemModel>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QMainWindow>

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

private:
    Ui::FileWindow *ui;
    QFileSystemModel *foldersModel;
    QFileSystemModel *filesModel;
    QItemSelectionModel *selectModel;
    QString formatFileSize(qint64);

};

#endif // FILEWINDOW_H
