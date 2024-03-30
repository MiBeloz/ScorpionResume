#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>

namespace SR {
  namespace ApplicationVersion {
    extern const QString versionMajor;
    extern const QString versionMinor;
    extern const QString versionMicro;
  } // namespace ApplicationVersion

  namespace GlobalVariables {
    extern QString homeDirOpenFile;
    extern bool isUTF8Encoding;
    extern QString defaultFileFormat;
  } // namespace GlobalVariables


  enum Error {
    OpenFile,
    EmptyFile,
    NoTypeOfProcessing,
    IncorrectHead,
    OneMoreTool,
    NoTool,
    IncorrectNumeration,
    EndProgram,
    FindX,
    FindY,
    FindZ,
    FindF,
    FindG,
  };

  enum Warning { TooFewFrames };
} // // namespace SR

#endif // DATA_H
