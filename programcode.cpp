#include "programcode.h"

ProgramCode::ProgramCode(QObject *parent)
    : QObject{parent},  m_countOfFrames(0)
{}

bool ProgramCode::isEmpty() const {
    return m_progamCode.isEmpty();
}

ProgramCode& ProgramCode::operator=(QStringList &&programCode) {
    m_progamCode = qMove(programCode);
    m_countOfFrames = 0;
    return *this;
}

void ProgramCode::checkCode() {
    removeNewlines();
    removeSpacesAndTabsFromBeginning();
    calcCountOfFrames();
}

uint32_t ProgramCode::getCountOfFrames() {
    return m_countOfFrames;
}

QStringList ProgramCode::getProgramCode()
{
    return m_progamCode;
}

void ProgramCode::forEach(std::function<void(QString&)> f) {
    for (int i = 0; i < m_progamCode.size(); ++i) {
        if (!m_progamCode[i].isEmpty()) {
            f(m_progamCode[i]);
        }
    }
}

void ProgramCode::removeNewlines() {
    forEach([](QString& frame) {
        if (frame[frame.size() - 1] == '\n') {
            frame = frame.mid(0, frame.size() - 1);
        }
    });
}

void ProgramCode::removeSpacesAndTabsFromBeginning() {
    forEach([](QString& frame) {
        while (frame[0] == static_cast<char>(32) || frame[0] == static_cast<char>(9)) {
            frame = frame.mid(1);
            if (frame.isEmpty()) {
                break;
            }
        }
    });
}

void ProgramCode::calcCountOfFrames() {
    forEach([&](QString& frame) {
        if (frame[0] == 'N') {
            ++m_countOfFrames;
            checkFrameNumber(frame, m_countOfFrames);
        }
    });
}

void ProgramCode::checkFrameNumber(QString &frame, uint32_t frameNumber) {
    std::string str = frame.toStdString();
    size_t n = str.find('N' + std::to_string(frameNumber) + ' ');
    if (n == std::string::npos) {
        emit sig_errorNumeration(Errors::Error::erIncorrectNumeration);
        return;
    }
}
