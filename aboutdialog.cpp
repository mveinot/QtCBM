#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "version.h"

aboutDialog::aboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);
    ui->version->setText("Version: "+QString(VERSION));
}

aboutDialog::~aboutDialog()
{
    delete ui;
}
