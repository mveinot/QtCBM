#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class settingsDialog;
}

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = 0);
    ~settingsDialog();

private slots:
    void on_browse_cbmctrl_clicked();

    void on_browse_cbmformat_clicked();

    void on_browse_cbmforng_clicked();

    void on_browse_d64copy_clicked();

private:
    Ui::settingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
