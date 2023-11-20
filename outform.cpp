#include "outform.h"
#include "ui_outform.h"

OutForm::OutForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutForm)
{
    ui->setupUi(this);
}

OutForm::~OutForm()
{
    delete ui;
}

void OutForm::lwOutClear()
{
    ui->lw_out->clear();
}

void OutForm::lwOutAddItem(QString item)
{
    ui->lw_out->addItem(item);
}

int OutForm::getLwOutCount()
{
    return ui->lw_out->count();
}

void OutForm::on_pb_close_clicked()
{
    close();
}

void OutForm::on_pb_save_clicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Сохранить"), "G:/", tr("MPF файлы (*.MPF)"));

    QFile out(path);
    if(!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream stream( &out );
    for (int i = 0; i < ui->lw_out->count(); ++i) {
        stream << ui->lw_out->item(i)->text() << endl;
    }
    out.close();
}
