#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QObject>
#include <QSpinBox>
#include <QFocusEvent>
#include <QDebug>

class MySpinBox : public QSpinBox
{
    Q_OBJECT
public:
    MySpinBox(QSpinBox *spB);

    void focusInEvent(QFocusEvent* e);

signals:
    void sig_inFocus();
};

#endif // MYSPINBOX_H
