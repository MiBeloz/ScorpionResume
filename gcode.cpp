#include "gcode.h"

GCode::GCode(QObject* parent) : QObject{parent} {}

GCode& GCode::operator=(QStringList&& GCode) {
  m_GCode = qMove(GCode);
  reset();
  return *this;
}

bool GCode::isEmpty() const { return m_GCode.isEmpty(); }

void GCode::checkCode() {
  removeNewlines();
  removeSpacesAndTabsFromBeginning();
  calcCountOfFrames();
}

uint32_t GCode::getCountOfFrames() { return m_countOfFrames; }

uint32_t GCode::getHead() {
  if (m_countHeadFrames == 0) {
    forEach<bool>([&](QString& frame) -> bool {
      static const std::regex regexPatternHead(R"(N[0-9]{1,}\s\;\([\-]{3,}\)\s?)");
      if (std::regex_match(frame.toStdString(), regexPatternHead)) {
        m_countHeadFrames = getFrameNumber(frame);
        return true;
      }
      return false;
    });
  }
  return m_countHeadFrames;
}

QMap<uint32_t, QString> GCode::getTypesOfProcessing() {
  if (m_typesOfProcessing.isEmpty()) {
    forEach<void>([&](QString& frame) {
      if (frameIsProcessingName(frame)) {
        QString typeOfProcessing = getTypeOfProcessing(frame);
        uint32_t frameOfProcessing = getFrameNumber(frame);
        m_typesOfProcessing[frameOfProcessing] = typeOfProcessing;
      }
    });
  }
  return m_typesOfProcessing;
}

QStringList GCode::getTools() {
  if (m_tools.isEmpty()) {
    forEach<void>([&](QString& frame) {
      if (frameIsTool(frame)) {
        QString tool = getTool(frame);
        m_tools.insert(tool);
      }
    });
  }
  return m_tools.values();
}

QStringList GCode::getProgramCode() { return m_GCode; }

void GCode::reset() {
  m_countOfFrames = 0;
  m_countHeadFrames = 0;
  m_typesOfProcessing.clear();
  m_tools.clear();
}

void GCode::removeNewlines() {
  forEach<void>([](QString& frame) {
    if (frame[frame.size() - 1] == '\n') {
      frame = frame.mid(0, frame.size() - 1);
    }
  });
}

void GCode::removeSpacesAndTabsFromBeginning() {
  forEach<void>([](QString& frame) {
    while (frame[0] == static_cast<char>(32) || frame[0] == static_cast<char>(9)) {
      frame = frame.mid(1);
      if (frame.isEmpty()) {
        break;
      }
    }
  });
}

void GCode::calcCountOfFrames() {
  forEach<void>([&](QString& frame) {
    if (frame[0] == 'N') {
      ++m_countOfFrames;
      if (!checkFrameNumber(frame, m_countOfFrames)) {
        emit sig_errorNumeration(Errors::Error::erIncorrectNumeration);
        return;
      }
    }
  });
}

bool GCode::checkFrameNumber(QString& frame, uint32_t frameNumber) {
  qsizetype n = frame.indexOf('N' + QString::number(frameNumber) + ' ');
  if (n != -1) {
    return true;
  }
  return false;
}

bool GCode::frameIsProcessingName(QString frame) {
  static const std::regex regexTypeOfProcessing(R"(N[0-9]{1,}\s\;\([0-9A-Z\-]*\s\-\s[0-9A-Z\-]*\)\s?)");
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

uint32_t GCode::getFrameNumber(QString frame) {
  qsizetype n = frame.indexOf(' ');
  if (n != -1) {
    frame = frame.mid(1, n);
    return frame.toUInt();
  }
  return 0;
}

bool GCode::frameIsTool(QString frame) {
  static const std::regex regexTool(R"(N[0-9]{1,}\sTC_CHANGETOOL\([0-9A-Z\-]{1,}\,[3-5]\,[0-9]{3,5}\)\s?)");
  if (std::regex_match(frame.toStdString(), regexTool)) {
    return true;
  }
  return false;
}

QString GCode::getTool(QString frame) {
  QString pattern = "TC_CHANGETOOL(";
  qsizetype n = frame.indexOf(pattern);
  if (n != -1) {
    frame = frame.mid(n + pattern.size());
    n = frame.indexOf(',');
    if (n != -1) {
      frame = frame.mid(0, n);
      return frame;
    }
  }
  return "";
}
