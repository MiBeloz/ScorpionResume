#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

#include "data.h"
#include "dictionary.h"

namespace Ui {
  class About;
}

class About : public QDialog {
  Q_OBJECT

public:
  explicit About(QWidget *parent = nullptr);
  ~About();

  void changeLanguage(Dictionary::Language language);

private slots:
  void on_pb_ok_clicked();

private:
  Ui::About *ui;
  Dictionary *pDictionary;

  void setLanhuage();
};

#endif // ABOUT_H
