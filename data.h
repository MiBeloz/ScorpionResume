#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>

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

namespace ProgressLoadingFile {
  enum OperationName { readingFile, processingFile, searchingTypesOfProcessingAndUsedTools, finish };

  extern const QVector<QString> progressOperation;
} // namespace ProgressLoadingFile

namespace ProgressGenerateFile {
  enum OperationName { searchXYZFG, generate, finish };

  extern const QVector<QString> progressGenerate;
} // namespace ProgressGenerateFile

namespace Errors {
  enum Error {
    erOpen,
    erEmptyFile,
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
  };
  enum Warning { wrnTooFewFrames };

  extern const QString errorMsg;
  extern const QString warningMsg;
  extern const QVector<QString> errorDescription;
  extern const QVector<QString> warningDescription;
} // namespace Errors

#endif // DATA_H
