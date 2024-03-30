#include "log.h"

Log* Log::mp_log = nullptr;
QString Log::m_filename = "resume.sr";

Log* Log::init(QObject* parent) {
  if (!mp_log) {
    mp_log = new Log(parent);
  }
  return mp_log;
}

void Log::setRecordSourceFilename(const QString &recordSourceFilename) {
  m_recordSourceFilename = recordSourceFilename;
}

void Log::setRecordPartName(const QString &recordPartName) {
  m_recordPartName = recordPartName;
}

void Log::setRecordFrame(const QString &recordFrame) {
  m_recordFrame = recordFrame;
}

void Log::clearRecord() {
  m_recordSourceFilename.clear();
  m_recordFrame.clear();
}

void Log::write() {
  QFile logfile(m_filename);
  if (!logfile.open(QIODevice::Append | QIODevice::Text)) {
    return;
  }
  QTextStream stream(&logfile);
  stream << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss")
         << "; SourceFilename: " << m_recordSourceFilename
         << "; PartName: " << m_recordPartName
         << "; Frame: " << m_recordFrame
         << Qt::endl;
  logfile.close();
}

Log::Log(QObject* parent) : QObject(parent) {}
