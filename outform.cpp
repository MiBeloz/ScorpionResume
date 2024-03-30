#include "outform.h"
#include "ui_outform.h"

OutForm::OutForm(QWidget *parent) : QDialog(parent), ui(new Ui::OutForm) {
  ui->setupUi(this);
  pDictionary = Dictionary::init();
  pLog = Log::init();
}

OutForm::~OutForm() {
  delete ui;
}

void OutForm::lwOutClear() {
  ui->lw_out->clear();
  mFilename.clear();
}

void OutForm::lwOutAddList(QStringList strList) {
    ui->lw_out->addItems(strList);
}

void OutForm::setFilename(QString filename) {
  mFilename = filename;
}

int OutForm::getLwOutCount() {
  return ui->lw_out->count();
}

void OutForm::changeLanguage() {
  ui->pb_save->setText(tr("%1").arg(pDictionary->translate(Dictionary::Translate::Save)));
  ui->pb_close->setText(tr("%1").arg(pDictionary->translate(Dictionary::Translate::Close)));
}

void OutForm::on_pb_close_clicked() {
  close();
}

void OutForm::on_pb_save_clicked() {
  QString path = QFileDialog::getSaveFileName(this,
                                              tr("%1").arg(pDictionary->translate(Dictionary::Translate::Save)),
                                              tr("%1").arg(mFilename),
                                              tr("%1 %2 (*.%1)").arg(SR::GlobalVariables::defaultFileFormat, pDictionary->translate(Dictionary::Translate::files)));

  QFile out(path);
  if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }
  QTextStream stream(&out);
  for (int i = 0; i < ui->lw_out->count(); ++i) {
    stream << ui->lw_out->item(i)->text() << Qt::endl;
  }
  out.close();

  pLog->write();
}
