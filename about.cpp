#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->lb_text->setText("Scorpion Resume\n"
                         "Версия " + QString::number(SCORPION_RESUME_VERSION_MAJOR) + '.' + QString::number(SCORPION_RESUME_VERSION_MINOR) + "\n\n" +
                         "Белозеров Михаил\n"
                         "2023\n"
                         "\n"
                         "Предназначена для возобновления программ\n"
                         "станка 'Scorpion' с определенного кадра.");
}

About::~About()
{
    delete ui;
}

void About::on_pushButton_clicked()
{
    close();
}
