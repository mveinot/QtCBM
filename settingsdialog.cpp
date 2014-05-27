#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>

settingsDialog::settingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDialog)
{
    ui->setupUi(this);

#ifdef Q_OS_OSX
    ui->cableFrame->setEnabled(true);
#endif

    settings = new QSettings("mvgrafx", "QtCBM");
    ui->input_cbmctrl->setText(settings->value("tools/cbmctrl", findCBMUtil("cbmctrl")).toString());
    ui->input_cbmforng->setText(settings->value("tools/cbmforng", findCBMUtil("cbmforng")).toString());
    ui->input_d64copy->setText(settings->value("tools/d64copy", findCBMUtil("d64copy")).toString());
    ui->input_cbmcopy->setText(settings->value("tools/cbmcopy", findCBMUtil("cbmcopy")).toString());
    ui->input_morse->setText(settings->value("tools/morse", findCBMUtil("morse")).toString());
    ui->input_cbmdevice->setValue(settings->value("deviceid", 8).toInt());
    ui->autoRefresh->setChecked(settings->value("autorefresh", true).toBool());
    ui->showCommands->setChecked(settings->value("showcmd", false).toBool());
    //ui->useC64font->setChecked(settings->value("usec64font", false).toBool());
    ui->genRandomDiskname->setChecked(settings->value("genrandomdisk", false).toBool());
    QString transfermode = settings->value("transfermode", "auto").toString();
    QString cableType = settings->value("cableType", "xum1541").toString();

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

    if (cableType == "xu1541")
        ui->cableXU->setChecked(true);
    if (cableType == "xum1541")
        ui->cableXUM->setChecked(true);
}

settingsDialog::~settingsDialog()
{
    delete ui;
}

QString settingsDialog::findCBMUtil(QString name)
{
#ifdef Q_OS_WIN
    return QStandardPaths::findExecutable(name+".exe");
#endif
#ifdef Q_OS_OSX
    return QCoreApplication::applicationDirPath()+"/bin/"+name;
#endif
#ifdef Q_OS_LINUX
    return QStandardPaths::findExecutable(name);
#endif
}

void settingsDialog::on_browse_cbmctrl_clicked()
{
    ui->input_cbmctrl->setText(QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find cbmctrl.exe"), "/", tr("Executable Files (*.exe)"))));
}

void settingsDialog::on_browse_cbmforng_clicked()
{
    ui->input_cbmforng->setText(QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find cbmforng.exe"), "/", tr("Executable Files (*.exe)"))));
}

void settingsDialog::on_browse_d64copy_clicked()
{
    ui->input_d64copy->setText(QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find d64copy.exe"), "/", tr("Executable Files (*.exe)"))));
}

void settingsDialog::on_browse_cbmcopy_clicked()
{
    ui->input_cbmcopy->setText(QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find cbmcopy.exe"), "/", tr("Executable Files (*.exe)"))));
}

void settingsDialog::on_browse_morse_clicked()
{
    ui->input_morse->setText(QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find morse.exe"), "/", tr("Executable Files (*.exe)"))));
}

void settingsDialog::on_buttonBox_accepted()
{
    settings->setValue("tools/cbmctrl", ui->input_cbmctrl->text());
    settings->setValue("tools/cbmforng", ui->input_cbmforng->text());
    settings->setValue("tools/d64copy", ui->input_d64copy->text());
    settings->setValue("tools/cbmcopy", ui->input_cbmcopy->text());
    settings->setValue("tools/morse", ui->input_morse->text());
    settings->setValue("deviceid", ui->input_cbmdevice->value());
    settings->setValue("transfermode", getTransferMode());
    settings->setValue("showcmd", ui->showCommands->isChecked());
    settings->setValue("autorefresh", ui->autoRefresh->isChecked());
    //settings->setValue("usec64font", ui->useC64font->isChecked());
    settings->setValue("genrandomdisk", ui->genRandomDiskname->isChecked());
    settings->setValue("cableType", getCableType());
    settings->sync();

    emit settingsChanged();
}

QString settingsDialog::getCableType()
{
    if (ui->cableXU->isChecked())
        return "xu1541";
    if (ui->cableXUM->isChecked())
        return "xum1541";
    return "xum1541";
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

void settingsDialog::on_cbmctrl_reset_clicked()
{
    ui->input_cbmctrl->setText(findCBMUtil("cbmctrl"));
}

void settingsDialog::on_d64copy_reset_clicked()
{
    ui->input_d64copy->setText(findCBMUtil("d64copy"));
}

void settingsDialog::on_cbmcopy_reset_clicked()
{
    ui->input_cbmcopy->setText(findCBMUtil("cbmcopy"));
}

void settingsDialog::on_cbmforng_reset_clicked()
{
    ui->input_cbmforng->setText(findCBMUtil("cbmforng"));
}

void settingsDialog::on_morse_reset_clicked()
{
    ui->input_morse->setText(findCBMUtil("morse"));
}
