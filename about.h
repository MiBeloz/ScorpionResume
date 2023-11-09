#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

#define SCORPION_RESUME_VERSION_MAJOR 0
#define SCORPION_RESUME_VERSION_MINOR 1

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = nullptr);
    ~About();

private slots:
    void on_pushButton_clicked();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
