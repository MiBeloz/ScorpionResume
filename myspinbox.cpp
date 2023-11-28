#include "myspinbox.h"

MySpinBox::MySpinBox(QSpinBox *spB)
{

}

void MySpinBox::focusInEvent(QFocusEvent *e)
{
    emit sig_inFocus();
}
