#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <QStandardPaths>

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);
    settings = new QSettings("mvgrafx", "QtCBM");
    ui->input_cbmctrl->setText(settings->value("tools/cbmctrl", QStandardPaths::findExecutable("cbmctrl.exe")).toString());
    ui->input_cbmforng->setText(settings->value("tools/cbmforng", QStandardPaths::findExecutable("cbmforng.exe")).toString());
    ui->input_d64copy->setText(settings->value("tools/d64copy", QStandardPaths::findExecutable("d64copy.exe")).toString());
    ui->input_cbmdevice->setValue(settings->value("deviceid", 8).toInt());
    ui->autoRefresh->setChecked(settings->value("autorefresh", true).toBool());
    ui->showCommands->setChecked(settings->value("showcmd", false).toBool());
    QString transfermode = settings->value("transfermode", "auto").toString();

    if (transfermode == "original")
        ui->trOriginal->setChecked(true);
    if (transfermode == "serial1")
        ui->trSerial1->setChecked(true);
    if (transfermode == "serial2")
        ui->trSerial2->setChecked(true);
    if (transfermode == "parallel")
        ui->trParallel->setChecked(true);
    if (transfermode == "auto")
        ui->trAuto->setChecked(true);
}

settingsDialog::~settingsDialog()
{
    delete ui;
}

void settingsDialog::on_browse_cbmctrl_clicked()
{
    ui->input_cbmctrl->setText(QFileDialog::getOpenFileName(this, tr("Find cbmctrl.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_browse_cbmforng_clicked()
{
    ui->input_cbmforng->setText(QFileDialog::getOpenFileName(this, tr("Find cbmforng.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_browse_d64copy_clicked()
{
    ui->input_d64copy->setText(QFileDialog::getOpenFileName(this, tr("Find d64copy.exe"), "/", tr("Executable Files (*.exe)")));
}

void settingsDialog::on_buttonBox_accepted()
{
    settings->setValue("tools/cbmctrl", ui->input_cbmctrl->text());
    settings->setValue("tools/cbmforng", ui->input_cbmforng->text());
    settings->setValue("tools/d64copy", ui->input_d64copy->text());
    settings->setValue("deviceid", ui->input_cbmdevice->value());
    settings->setValue("transfermode", getTransferMode());
    settings->setValue("showcmd", ui->showCommands->isChecked());
    settings->setValue("autorefresh", ui->autoRefresh->isChecked());
    settings->sync();

    emit settingsChanged();
}

QString settingsDialog::getTransferMode()
{
    if (ui->trOriginal->isChecked())
        return "original";
    if (ui->trSerial1->isChecked())
        return "serial1";
    if (ui->trSerial2->isChecked())
        return "serial2";
    if (ui->trParallel->isChecked())
        return "parallel";
    if (ui->trAuto->isChecked())
        return "auto";
    return "auto";
}
