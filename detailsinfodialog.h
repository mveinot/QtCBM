#ifndef DETAILSINFODIALOG_H
#define DETAILSINFODIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class detailsInfoDialog;
}

class detailsInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit detailsInfoDialog(QWidget *parent = 0);
    void setDetailText(QString);
    void setText(QString);
    ~detailsInfoDialog();

private:
    Ui::detailsInfoDialog *ui;
};

#endif // DETAILSINFODIALOG_H
