#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    QPixmap pixmap(":/picture.png");
    pixmap = pixmap.scaled(128, 128);
    ui->lb_ico->setPixmap(pixmap);

    ui->lb_text->setText("\tScorpion Resume\n"
                         "\tВерсия " + versionMajor + '.' + versionMinor + "." + versionMicro + "\n\n" +
                         "\tБелозеров Михаил\n"
                         "\t2023\n"
                         "\n"
                         "\tПредназначена для возобновления программ\n"
                         "\tстанка 'Scorpion' с определенного кадра.");
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    close();
}
