#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QVector>

namespace ApplicationVersion {
    extern QString versionMajor;
    extern QString versionMinor;
    extern QString versionMicro;
}

namespace AboutDescription {
    extern QString description;
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
        searchingTypesOfProcessing,
        searchingUsedTools,
        finish
    };

    extern QVector<QString> progressName;
}

#endif // DATA_H
