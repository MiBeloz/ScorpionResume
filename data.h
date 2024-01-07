#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>

namespace ApplicationVersion {
  extern const QString versionMajor;
  extern const QString versionMinor;
  extern const QString versionMicro;
} // namespace ApplicationVersion

namespace AboutDescription {
  extern const QString description;
}

namespace GlobalVariables {
  extern QString homeDirOpenFile;
  extern bool isUTF8Encoding;
  extern QString defaultFileFormat;
} // namespace GlobalVariables

namespace ProgressLoadingFile {
  enum OperationName { readingFile = 0, processingFile, searchingTypesOfProcessingAndUsedTools, finish };

  extern const QVector<QString> progressOperationName;
} // namespace ProgressLoadingFile

namespace Errors {
  enum Error {
    erOpen = 0,
    erNoTypeOfProcessing,
    erIncorrectHead,
    erOneMoreTool,
    erNoTool,
    erIncorrectNumeration,
    erEndProgram,
    erFindX,
    erFindY,
    erFindZ,
    erFindF,
    erFindG,
    erOutFile
  };
  enum Warning { wrnEmptyFile = 0, wrnTooFewFrames };

  extern const QString errorMsg;
  extern const QString warningMsg;
  extern const QVector<QString> errorDescription;
  extern const QVector<QString> warningDescription;
} // namespace Errors

namespace FileOperations {
  extern const QString open;
  extern const QString save;
  extern const QString files;
} // namespace FileOperations

#endif // DATA_H
