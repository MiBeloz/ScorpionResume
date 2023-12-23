#include "programcode.h"

ProgramCode::ProgramCode(QObject *parent)
    : QObject{parent}
{}

bool ProgramCode::isEmpty() const {
    return m_progamCode.isEmpty();
}

ProgramCode& ProgramCode::operator=(QStringList &&programCode) {
    m_progamCode = qMove(programCode);
    return *this;
}

void ProgramCode::toQStringList(QStringList &stringList) {
    stringList = m_progamCode;
}
