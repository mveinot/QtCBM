#include "detailsinfodialog.h"
#include "ui_detailsinfodialog.h"

detailsInfoDialog::detailsInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::detailsInfoDialog)
{
    ui->setupUi(this);
    QStyle *style = QApplication::style();
    int iconSize = style->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, ui->icon);
    QIcon tmpIcon = style->standardIcon(QStyle::SP_MessageBoxInformation, 0, ui->icon);
    ui->icon->setPixmap(tmpIcon.pixmap(iconSize, iconSize));

    this->setWindowTitle(QApplication::applicationName());
}

void detailsInfoDialog::setDetailText(QString details)
{
    ui->detailPane->setText(details);
}

void detailsInfoDialog::setDetailText(QStringList details)
{
    ui->detailPane->setText(details.join('\n'));
}

void detailsInfoDialog::setText(QString text)
{
    ui->DialogText->setText(text);
}

detailsInfoDialog::~detailsInfoDialog()
{
    delete ui;
}
