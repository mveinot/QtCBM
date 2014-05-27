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
    ui->d64contents->hideColumn(3);
    QFont c64font = QFont("C64 Elite Mono", 12);
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
    QByteArray selectedFiles;

    this->setWindowTitle("Contents of "+fi.completeBaseName()+"."+fi.completeSuffix());

    if (fi.exists())
    {
        selectedFiles = CBMroutines::list_dir(filename);

        QList<QByteArray> dirlist = selectedFiles.split('\n');

        for (int i = 0; i < dirlist.count(); i++)
        {
            QList<QByteArray> line = dirlist.at(i).split('|');
            if (line.at(0) == "entry")
            {
                QString entry = CBMroutines::stringToPETSCII(line.at(2),true,true);

                QTreeWidgetItem *item = new QTreeWidgetItem();
                item->setText(0, CBMroutines::stringToPETSCII(QString(line.at(1)),true));
                item->setText(1, CBMroutines::stringToPETSCII(entry, true));
                item->setText(2, CBMroutines::stringToPETSCII(QString(line.at(3)).toUpper(),true));
                item->setData(3,Qt::DisplayRole, line.at(2));
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
    QByteArray selectedItems;
    QAbstractItemModel *model = ui->d64contents->model();
    QModelIndexList index = ui->d64contents->selectionModel()->selectedRows(3);

    for (int i = 0; i < index.count(); i++)
    {
        selectedItems.append(model->data(index.at(i)).toByteArray());
        selectedItems.append('\n');
    }
    emit sendSelectedContents(selectedItems);

    this->close();
}
