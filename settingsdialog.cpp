#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);
}

settingsDialog::~settingsDialog()
{
    delete ui;
}

void settingsDialog::on_browse_cbmctrl_clicked()
{
    ui->input_cbmctrl->setText(QFileDialog::getOpenFileName(this, tr("Find cbmctrl.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_browse_cbmformat_clicked()
{
    ui->input_cbmformat->setText(QFileDialog::getOpenFileName(this, tr("Find cbmformat.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_browse_cbmforng_clicked()
{
    ui->input_cbmforng->setText(QFileDialog::getOpenFileName(this, tr("Find cbmforng.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_browse_d64copy_clicked()
{
    ui->input_d64copy->setText(QFileDialog::getOpenFileName(this, tr("Find d64copy.exe"), "/", tr("Executable Files (*.exe)")));
}
