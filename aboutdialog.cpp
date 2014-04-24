#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    ui->qtcbm->setFont(QFont("C64 Pro Mono", 65));
    ui->version->setFont(QFont("C64 Pro Mono", 12));
    ui->version->setText("Version: "+QString(__QTCBM_VERSION__));
}

aboutDialog::~aboutDialog()
{
    delete ui;
}
