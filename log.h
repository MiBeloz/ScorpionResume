#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

class Log : public QObject {
  Q_OBJECT

public:
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

  static Log* init(QObject* parent = nullptr);
  void setRecordSourceFilename(const QString& recordSourceFilename);
  void setRecordPartName(const QString& recordPartName);
  void setRecordFrame(const QString& recordFrame);
  void clearRecord();
  void write();

private:
  explicit Log(QObject* parent);
  static Log* mp_log;
  static QString m_filename;
  QString m_recordSourceFilename;
  QString m_recordPartName;
  QString m_recordFrame;
};

#endif // LOG_H
