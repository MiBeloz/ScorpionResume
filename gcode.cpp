#include "gcode.h"

GCode::GCode(QObject* parent) : QObject(parent) {}

bool GCode::addGCode(QStringList GCodeList) {
  reset();
  m_GCode = GCodeList;
  if (!m_GCode.isEmpty() && !m_GCode[0].isEmpty()) {
    m_GCode.push_front("");
  }
  removeNewlines();
  removeSpacesAndTabsFromBeginning();
  removeEmptyFrames();
  return (calcCountOfFrames() && checkEndProgram() && calcCountHeadFrames());
}

int GCode::getCountOfFrames() {
  return m_countOfFrames;
}

int GCode::getHead() {
  return m_countHeadFrames;
}

QString GCode::getPartName() {
  static const std::regex regexPartName(R"(N[0-9]\s?\;\s?Part\sname\s\-)");
  QString result;
  for (const auto& line : m_GCode) {
    if (std::regex_search(line.toStdString(), regexPartName)) {
      result = line;
      break;
    }
  }

  if (qsizetype n = result.indexOf("Part name -"); n != -1) {
    result = result.mid(n + 11);
  }
  if (qsizetype n = result.lastIndexOf('-'); n != -1) {
    result = result.mid(0, n);
  }

  return result;
}

QMap<int, QString> GCode::getTypesOfProcessing() {
  if (m_typesOfProcessing.isEmpty()) {
    forEach([&](QString& frame) {
      if (frameIsProcessingName(frame)) {
        QString typeOfProcessing = getTypeOfProcessing(frame);
        int frameOfProcessing = getFrameNumber(frame);
        m_typesOfProcessing[frameOfProcessing] = typeOfProcessing;
      }
    });
  }
  return m_typesOfProcessing;
}

QStringList GCode::getTools() {
  if (m_tools.isEmpty()) {
    forEach([&](QString& frame) {
      if (frameIsTool(frame)) {
        QString tool = getTool(frame);
        m_tools.insert(tool);
      }
    });
  }
  return m_tools.values();
}

QStringList GCode::getProgramCode() {
  return m_GCode;
}

void GCode::reset() {
  m_GCode.clear();
  m_countOfFrames = 0;
  m_countHeadFrames = 0;
  m_typesOfProcessing.clear();
  m_tools.clear();
}

void GCode::generateOutProgramCode(GCode* gcode, int stopFrame) {
  m_GCodeOut.clear();

  QVector<double> commands;
  commands.resize(5);
  commands[eCommand::X] = findValue(stopFrame, 'X').value_or(BadValue);
  commands[eCommand::Y] = findValue(stopFrame, 'Y').value_or(BadValue);
  commands[eCommand::Z] = findValue(stopFrame, 'Z').value_or(BadValue);
  commands[eCommand::F] = findValue(stopFrame, 'F').value_or(BadValue);
  commands[eCommand::G] = findValue(stopFrame, 'G').value_or(BadValue);

  if (commands[eCommand::X] == BadValue) {
    emit sig_error(SR::FindX, false);
    return;
  }
  if (commands[eCommand::Y] == BadValue) {
    emit sig_error(SR::FindY, false);
    return;
  }
  if (commands[eCommand::Z] == BadValue) {
    emit sig_error(SR::FindZ, false);
    return;
  }
  if (commands[eCommand::F] == BadValue) {
    emit sig_error(SR::FindF, false);
    return;
  }
  if (commands[eCommand::G] == BadValue) {
    emit sig_error(SR::FindG, false);
    return;
  }

  qDebug() << "Head = " + QString::number(gcode->getHead());
  qDebug() << "X = " + QString::number(commands[eCommand::X]);
  qDebug() << "Y = " + QString::number(commands[eCommand::Y]);
  qDebug() << "Z = " + QString::number(commands[eCommand::Z]);
  qDebug() << "F = " + QString::number(commands[eCommand::F]);
  qDebug() << "G = " + QString::number(commands[eCommand::G]);

  for (int i = 0; i < m_countHeadFrames; ++i) {
    m_GCodeOut.push_back(m_GCode[i]);
  }

  for (auto it = m_typesOfProcessing.end(); it != m_typesOfProcessing.begin();) {
    --it;
    if (it.key() < stopFrame) {
      QString str = ";(";
      for (int j = 0; j < it.value().size() + 13; ++j) {
        str += "-";
      }
      str += ")";
      m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + str + ' ');
      m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " ;(PROCESSING - " + it.value() + ") ");
      m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + str + ' ');

      break;
    }
  }

  m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " G1 " + "X" + QString::number(commands[eCommand::X]) + " Y"
                       + QString::number(commands[eCommand::Y]) + " F4000 ");
  m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " Z" + QString::number(commands[eCommand::Z]) + ' ');

  QString nextFrame = deleteFrameNumber(m_GCode[stopFrame]);
  if (isFrameContains(nextFrame, "G")) {
    m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + nextFrame);
  } else {
    m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + 'G' + QString::number(commands[eCommand::G]) + ' ' + nextFrame);
  }
  if (!isFrameContains(nextFrame, "F")) {
    m_GCodeOut[m_GCodeOut.size() - 1] += 'F' + QString::number(commands[eCommand::F]) + ' ';
  }

  for (int i = stopFrame + 1; i < m_GCode.size(); ++i) {
    if (i == m_GCode.size() - 1) {
      m_GCodeOut.push_back(m_GCode[i]);
    } else {
      QString sourceFrame = deleteFrameNumber(m_GCode[i]);
      m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + sourceFrame);
    }
  }
}

QStringList GCode::getOutProgramCode() {
  return m_GCodeOut;
}

void GCode::forEach(std::function<void(QString& frame)> f) {
  for (int i = 1; i < m_GCode.size(); ++i) {
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

void GCode::removeEmptyFrames() {
  for (int i = m_GCode.size() - 1; i > 0; --i) {
    if (m_GCode[i].isEmpty()) {
      m_GCode.removeAt(i);
    }
  }
}

bool GCode::calcCountOfFrames() {
  bool calcOfFramesIsOk = true;
  forEach([&](QString& frame) {
    if (frame[0] == 'N') {
      ++m_countOfFrames;
      if (!checkFrameNumber(frame, m_countOfFrames)) {
        reset();
        calcOfFramesIsOk = false;
        emit sig_error(SR::Error::IncorrectNumeration);
        return;
      }
    }
  });
  return calcOfFramesIsOk;
}

bool GCode::calcCountHeadFrames() {
  bool calcHeadIsOk = true;
  if (m_countHeadFrames == 0) {
    for (int i = 0; i < m_GCode.size(); ++i) {
      if (!m_GCode[i].isEmpty()) {
        static const std::regex regexPatternHead(R"(N[0-9]{1,}\s\;\([\-]{3,}\)\s?)");
        if (std::regex_match(m_GCode[i].toStdString(), regexPatternHead)) {
          m_countHeadFrames = getFrameNumber(m_GCode[i]);
          if (m_countHeadFrames != 12) {
            calcHeadIsOk = false;
            emit sig_error(SR::Error::IncorrectHead);
          }
          break;
        }
      }
    }
  }
  return calcHeadIsOk;
}

bool GCode::checkFrameNumber(QString& frame, int frameNumber) {
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

int GCode::getFrameNumber(QString frame) {
  qsizetype n = frame.indexOf(' ');
  if (n != -1) {
    frame = frame.mid(1, n);
    return frame.toInt();
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

std::optional<double> GCode::findValue(int startFrame, QChar command) {
  double result{0};
  for (int i = startFrame - 1; i > 0; --i) {
    QString sourceFrame = deleteFrameNumber(m_GCode[i]);
    qsizetype n = sourceFrame.lastIndexOf(command);
    if (n != -1) {
      sourceFrame = sourceFrame.mid(n + 1);

      int m = 0;
      if (sourceFrame[m] == '-') {
        ++m;
      }

      while (m < sourceFrame.size() && (sourceFrame[m].isDigit() || sourceFrame[m] == '.')) {
        ++m;
      }

      sourceFrame = sourceFrame.left(m);
      result = sourceFrame.toDouble();
      return result;
    }
  }
  return std::nullopt;
}

QString GCode::deleteFrameNumber(QString frame) {
  std::string strString = frame.toStdString();
  auto it = std::find(strString.begin(), strString.end(), ' ');
  if (it != strString.end()) {
    while (*it == ' ') {
      ++it;
    }
    strString.erase(strString.begin(), it);
  }
  return QString::fromStdString(strString);
}

bool GCode::isFrameContains(QString frame, QString str) {
  qsizetype n = frame.indexOf(str);
  if (n != -1) {
    return true;
  }
  return false;
}

bool GCode::checkEndProgram() {
  static const std::regex regexEndProgram(R"(M30\s?)");
  if (std::regex_match(m_GCode[m_GCode.size() - 1].toStdString(), regexEndProgram)) {
    return true;
  }
  emit sig_error(SR::Error::EndProgram);
  return false;
}
