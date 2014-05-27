#ifndef SELECTD64FILESDIALOG_H
#define SELECTD64FILESDIALOG_H

#include <QDialog>

namespace Ui {
class Selectd64FilesDialog;
}

class Selectd64FilesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Selectd64FilesDialog(QWidget *parent = 0);
    void setD64File(QString filename);
    ~Selectd64FilesDialog();

signals:
    void sendSelectedContents(QByteArray);

private slots:
    void on_cancelBtn_clicked();

    void on_extractBtn_clicked();

private:
    Ui::Selectd64FilesDialog *ui;
};

#endif // SELECTD64FILESDIALOG_H
