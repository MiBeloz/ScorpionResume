#ifndef GCODE_H
#define GCODE_H

#include <QDebug>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <optional>
#include <regex>

#include "data.h"

class GCode : public QObject {
  Q_OBJECT

public:
  explicit GCode(QObject* parent = nullptr);

  bool addGCode(QStringList GCodeList);
  uint32_t getCountOfFrames();
  uint32_t getHead();
  QMap<uint32_t, QString> getTypesOfProcessing();
  QStringList getTools();
  QStringList getProgramCode();
  void reset();

  void generateOutProgramCode(GCode* gcode, uint32_t stopFrame);
  QStringList getOutProgramCode();

signals:
  void sig_error(Errors::Error, bool resetAll = true);

private:
  enum eCommand { X, Y, Z, F, G };
  constexpr static double BadValue = -100000;
  QStringList m_GCode;
  QStringList m_GCodeOut;
  uint32_t m_countOfFrames;
  uint32_t m_countHeadFrames;
  QMap<uint32_t, QString> m_typesOfProcessing;
  QSet<QString> m_tools;

  void forEach(std::function<void(QString&)> f);
  void removeNewlines();
  void removeSpacesAndTabsFromBeginning();
  void removeEmptyFrames();
  bool calcCountOfFrames();
  bool calcCountHeadFrames();
  bool checkFrameNumber(QString& frame, uint32_t frameNumber);
  bool frameIsProcessingName(QString frame);
  QString getTypeOfProcessing(QString frame);
  uint32_t getFrameNumber(QString frame);
  bool frameIsTool(QString frame);
  QString getTool(QString frame);
  std::optional<double> findValue(int startFrame, QChar command);
  QString deleteFrameNumber(QString frame);
  bool isFrameContains(QString frame, QString str);
  bool checkEndProgram();
};

#endif // GCODE_H
