#include "gcode.h"

GCode::GCode(QObject *parent)
    : QObject{parent},  m_countOfFrames{0}, m_countHeadFrames{0}
{}

GCode& GCode::operator=(QStringList &&GCode) {
    m_GCode = qMove(GCode);
    m_countOfFrames = 0;
    m_countHeadFrames = 0;
    return *this;
}

bool GCode::isEmpty() const {
    return m_GCode.isEmpty();
}

void GCode::checkCode() {
    removeNewlines();
    removeSpacesAndTabsFromBeginning();
    calcCountOfFrames();
}

uint32_t GCode::getCountOfFrames() {
    return m_countOfFrames;
}

QMap<uint32_t, QString> GCode::getTypesOfProcessing() {
    QMap<uint32_t, QString> typesOfProcessing;

    forEach([&](QString& frame){
        if (frameIsProcessingName(frame)) {
            QString typeOfProcessing = getTypeOfProcessing(frame);
            uint32_t frameOfProcessing = getFrameOfProcessing(frame);
            typesOfProcessing[frameOfProcessing] = typeOfProcessing;
        }
    });

    return typesOfProcessing;
}

QStringList GCode::getProgramCode() {
    return m_GCode;
}

void GCode::forEach(std::function<void(QString&)> f) {
    for (int i = 0; i < m_GCode.size(); ++i) {
        if (!m_GCode[i].isEmpty()) {
            f(m_GCode[i]);
        }
    }
}

void GCode::removeNewlines() {
    forEach([](QString& frame) {
        if (frame[frame.size() - 1] == '\n') {
            frame = frame.mid(0, frame.size() - 1);
        }
    });
}

void GCode::removeSpacesAndTabsFromBeginning() {
    forEach([](QString& frame) {
        while (frame[0] == static_cast<char>(32) || frame[0] == static_cast<char>(9)) {
            frame = frame.mid(1);
            if (frame.isEmpty()) {
                break;
            }
        }
    });
}

void GCode::calcCountOfFrames() {
    forEach([&](QString& frame) {
        if (frame[0] == 'N') {
            ++m_countOfFrames;
            checkFrameNumber(frame, m_countOfFrames);
        }
    });
}

void GCode::checkFrameNumber(QString &frame, uint32_t frameNumber) {
    std::string str = frame.toStdString();
    size_t n = str.find('N' + std::to_string(frameNumber) + ' ');
    if (n == std::string::npos) {
        emit sig_errorNumeration(Errors::Error::erIncorrectNumeration);
        return;
    }
}

bool GCode::frameIsProcessingName(QString frame) {
    static const std::regex regexTypeOfProcessing (R"(N[0-9]*\s\;\([0-9A-Z\-]*\s\-\s[0-9A-Z\-]*\)\s)");
    if (std::regex_match(frame.toStdString(), regexTypeOfProcessing)) {
        return true;
    }
    return false;
}

QString GCode::getTypeOfProcessing(QString frame) {
    qsizetype n = frame.indexOf(" - ");
    if (n != -1) {
        frame = frame.mid(n + 3);
        n = frame.indexOf(')');
        if (n != -1) {
            frame = frame.mid(0, n);
        }
    }
    return frame;
}

uint32_t GCode::getFrameOfProcessing(QString frame) {
    qsizetype n = frame.indexOf(' ');
    if (n != -1) {
        frame = frame.mid(1, n);
        return frame.toUInt();
    }
    return 0;
}
