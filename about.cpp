#include "about.h"
#include "ui_about.h"

About::About(QWidget* parent) : QDialog(parent), ui(new Ui::About) {
  ui->setupUi(this);
  pDictionary = Dictionary::init();

  QPixmap pixmap(":/picture.png");
  pixmap = pixmap.scaled(128, 128);
  ui->lb_ico->setPixmap(pixmap);
}

About::~About() {
  delete ui;
}

void About::changeLanguage() {
  ui->lb_text->setText(tr("\tScorpion Resume\n"
                          "\t%1: %2.%3.%4\n\n"
                          "\tBelozerov Mihail\n"
                          "\t2024\n"
                          "\n"
                          "%5.")
                           .arg(pDictionary->translate(Dictionary::Translate::Version),
                              SR::ApplicationVersion::versionMajor,
                              SR::ApplicationVersion::versionMinor,
                              SR::ApplicationVersion::versionMicro,
                              pDictionary->translate(Dictionary::Translate::ProgramDescription)));
}

void About::on_pb_ok_clicked() {
  close();
}
