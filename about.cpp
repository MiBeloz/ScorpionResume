#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) : QDialog(parent), ui(new Ui::About) {
  ui->setupUi(this);
  pDictionary = new Dictionary(this);

  QPixmap pixmap(":/picture.png");
  pixmap = pixmap.scaled(128, 128);
  ui->lb_ico->setPixmap(pixmap);
}

About::~About() {
  delete ui;
}

void About::changeLanguage(Dictionary::Language language) {
  pDictionary->setLanguage(language);
  setLanhuage();
}

void About::on_pb_ok_clicked() {
  close();
}

void About::setLanhuage() {
  ui->lb_text->setText(tr("\tScorpion Resume\n"
                          "\t%1: %2.%3.%4\n\n"
                          "\tBelozerov Mihail\n"
                          "\t2024\n"
                          "\n"
                          "\t%5.")
                           .arg(pDictionary->translateString("Version"), ApplicationVersion::versionMajor, ApplicationVersion::versionMinor,
                                ApplicationVersion::versionMicro,
                                pDictionary->translateString("Designed to resume 'Scorpion' machine programs from a specific frame")));
}
