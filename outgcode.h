#ifndef OUTGCODE_H
#define OUTGCODE_H

#include <QDebug>
#include <QObject>

#include <gcode.h>

class OutGCode : public QObject {
  Q_OBJECT
public:
  explicit OutGCode(QObject *parent = nullptr);

  void reset();
  void generate(GCode *gcode, uint32_t stopFrame);
  QStringList getProgramCode();

signals:
  void sig_incrementProgressBar();
  void sig_errorFindValue(Errors::Error);

private:
  QStringList m_GCodeOut;
  bool m_outCodeReady;

  bool findValue(GCode *gcode, double &axe, int startFrame, QChar command, bool checkIJK);
  QString deleteFrameNumber(QString frame);
};

#endif // OUTGCODE_H
