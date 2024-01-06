#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QFocusEvent>
#include <QKeyEvent>
#include <QObject>
#include <QSpinBox>

class MySpinBox : public QSpinBox {
  Q_OBJECT

public:
  explicit MySpinBox(QSpinBox *);

  void focusInEvent(QFocusEvent *event);
  void keyPressEvent(QKeyEvent *event);

signals:
  void sig_inFocus();
  void sig_enterPress();
};

#endif // MYSPINBOX_H
