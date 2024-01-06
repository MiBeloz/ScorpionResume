#include "myspinbox.h"

MySpinBox::MySpinBox(QSpinBox *) {}

void MySpinBox::focusInEvent(QFocusEvent *event) {
  QSpinBox::focusInEvent(event);
  emit sig_inFocus();
}

void MySpinBox::keyPressEvent(QKeyEvent *event) {
  QSpinBox::keyPressEvent(event);
  if (event->key() == Qt::Key_Enter) {
    emit sig_enterPress();
  }
}
