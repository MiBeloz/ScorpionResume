#ifndef OUTFORM_H
#define OUTFORM_H

#include <QDialog>
#include <QFileDialog>
#include <QTextStream>

#include "data.h"

namespace Ui {
class OutForm;
}

class OutForm : public QDialog
{
    Q_OBJECT

public:
    explicit OutForm(QWidget *parent = nullptr);
    ~OutForm();

    void lwOutClear();
    void lwOutAddItem(QString item);
    int getLwOutCount();

private slots:
    void on_pb_close_clicked();

    void on_pb_save_clicked();

private:
    Ui::OutForm *ui;
};

#endif // OUTFORM_H
