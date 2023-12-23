#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QObject>
#include <QSpinBox>
#include <QFocusEvent>
#include <QKeyEvent>

class MySpinBox : public QSpinBox {
    Q_OBJECT

public:
    explicit MySpinBox(QSpinBox*);

    void focusInEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void sig_inFocus();
    void sig_enterPress();
};

#endif // MYSPINBOX_H
