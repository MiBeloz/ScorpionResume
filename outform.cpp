#include "outform.h"
#include "ui_outform.h"

OutForm::OutForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutForm)
{
    ui->setupUi(this);
    pDictionary = new Dictionary(this, Dictionary::Language::russian);
}

OutForm::~OutForm() {
    delete ui;
}

void OutForm::lwOutClear() {
    ui->lw_out->clear();
}

void OutForm::lwOutAddList(QStringList strList) {
    ui->lw_out->addItems(strList);
}

int OutForm::getLwOutCount() {
    return ui->lw_out->count();
}

void OutForm::on_pb_close_clicked() {
    close();
}

void OutForm::on_pb_save_clicked() {
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("%1").arg(pDictionary->translateString(FileOperations::save)),
                                                tr("%1").arg(GlobalVariables::homeDirOpenFile),
                                                tr("%1 %2 (*.%1)").arg(GlobalVariables::defaultFileFormat, pDictionary->translateString(FileOperations::files)));

    QFile out(path);
    if(!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream stream( &out );
    for (int i = 0; i < ui->lw_out->count(); ++i) {
        stream << ui->lw_out->item(i)->text() << Qt::endl;
    }
    out.close();
}
