#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) : QDialog(parent), ui(new Ui::About) {
  ui->setupUi(this);

  QPixmap pixmap(":/picture.png");
  pixmap = pixmap.scaled(128, 128);
  ui->lb_ico->setPixmap(pixmap);

  ui->lb_text->setText(AboutDescription::description);
}

About::~About() { delete ui; }

void About::on_pb_ok_clicked() { close(); }
