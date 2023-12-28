#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>

namespace ApplicationVersion {
    extern const QString versionMajor;
    extern const QString versionMinor;
    extern const QString versionMicro;
}

namespace AboutDescription {
    extern const QString description;
}

namespace GlobalVariables {
    extern QString homeDirOpenFile;
    extern bool isUTF8Encoding;
    extern QString defaultFileFormat;
}

namespace ProgressLoadingFile {
    enum OperationName {
        readingFile                 = 0,
        processingFile,
        searchingTypesOfProcessingAndUsedTools,
        finish
    };

    extern const QVector<QString> progressOperationName;
}

namespace Errors {
    enum Error {
        erOpen      = 0,
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
        erFindG
    };

    extern const QString error;
    extern const QVector<QString> errorDescription;
}

namespace FileOperations {
    extern const QString open;
    extern const QString save;
    extern const QString files;
}

#endif // DATA_H
