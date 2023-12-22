#include "data.h"

namespace ApplicationVersion {
    QString versionMajor = "2";
    QString versionMinor = "2";
    QString versionMicro = "0";
}

namespace AboutDescription {
    QString description = "\tScorpion Resume\n"
                          "\tВерсия " + ApplicationVersion::versionMajor + '.' + ApplicationVersion::versionMinor + "." + ApplicationVersion::versionMicro + "\n\n" +
                          "\tБелозеров Михаил\n"
                          "\t2023\n"
                          "\n"
                          "\tПредназначена для возобновления программ\n"
                          "\tстанка 'Scorpion' с определенного кадра.";
}

namespace GlobalVariables {
    QString homeDirOpenFile { "G:/" };
    bool isUTF8Encoding { false };
    QString defaultFileFormat = "MPF";
}

namespace ProgressLoadingFile {
    QVector<QString> progressName {
        "Читаю файл...",
        "Обрабатываю файл...",
        "Ищу типы обработок..."
        "Ищу используемые инструменты..."
        "Готово!"
    };
}
