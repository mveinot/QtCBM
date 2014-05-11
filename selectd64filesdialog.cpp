#include <QAbstractItemModel>
#include <QFileInfo>
#include <QtDebug>

#include "cbmroutines.h"
#include "selectd64filesdialog.h"
#include "ui_selectd64filesdialog.h"

Selectd64FilesDialog::Selectd64FilesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Selectd64FilesDialog)
{
    ui->setupUi(this);

    ui->d64contents->header()->resizeSection(0, 75);
    ui->d64contents->header()->resizeSection(1, 350);
    ui->d64contents->header()->resizeSection(2, 40);
    QFont c64font = QFont("C64 Pro Mono", 12);
    ui->d64contents->setStyleSheet("QTreeWidget {background-color: #4E2EDE; color: #A7A1FD; }");
    ui->d64contents->setFont(c64font);
}

Selectd64FilesDialog::~Selectd64FilesDialog()
{
    delete ui;
}

void Selectd64FilesDialog::setD64File(QString filename)
{
    QFileInfo fi(filename);
    QStringList selectedFiles;
    this->setWindowTitle("Contents of "+fi.completeBaseName()+"."+fi.completeSuffix());

    if (fi.exists())
    {
        selectedFiles = CBMroutines::list_dir(filename);

        for (int i = 0; i < selectedFiles.count(); i++)
        {
            QRegExp rxDirEntry("(\\d+)\\s*\"(.*)\"\\s+(\\S\\S\\S)");

            QString regstring = selectedFiles.at(i);

            if (rxDirEntry.indexIn(regstring) >= 0)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, rxDirEntry.cap(1));
                item->setText(1, rxDirEntry.cap(2).toUpper());
                item->setText(2, rxDirEntry.cap(3).toUpper());
                ui->d64contents->addTopLevelItem(item);
            }
        }
        ui->d64contents->resizeColumnToContents(0);
    }
}

void Selectd64FilesDialog::on_cancelBtn_clicked()
{
    this->close();
}

void Selectd64FilesDialog::on_extractBtn_clicked()
{
    QStringList selectedItems;
    QAbstractItemModel *model = ui->d64contents->model();
    QModelIndexList index = ui->d64contents->selectionModel()->selectedRows(1);

    for (int i = 0; i < index.count(); i++)
    {
        selectedItems << model->data(index.at(i)).toString();
    }
    emit sendSelectedContents(selectedItems);

    this->close();
}
