#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QString>

namespace Ui {
class settingsDialog;
}

class settingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDialog(QWidget *parent = 0);
    ~settingsDialog();

signals:
    void settingsChanged();

private slots:
    void on_browse_cbmctrl_clicked();

    //void on_browse_cbmformat_clicked();

    void on_browse_cbmforng_clicked();

    void on_browse_d64copy_clicked();

    void on_buttonBox_accepted();

    void on_browse_cbmcopy_clicked();

    void on_browse_morse_clicked();

private:
    Ui::settingsDialog *ui;
    QSettings *settings;
    QString getTransferMode();
};

#endif // SETTINGSDIALOG_H
