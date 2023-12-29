#include "data.h"

namespace ApplicationVersion {
    const QString versionMajor = "2";
    const QString versionMinor = "2";
    const QString versionMicro = "6";
}

namespace AboutDescription {
    const QString description = "\tScorpion Resume\n"
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
    const QVector<QString> progressOperationName {
        "Читаю файл...",
        "Обрабатываю файл...",
        "Ищу типы обработок и используемые инструменты...",
        "Готово!"
    };
}

namespace Errors {
    const QString error = "Error!";
    const QVector<QString> errorDescription {
        "Ошибка открытия файла!",
        "Программа повреждена!\nНе найдено ни одного вида обработки.",
        "Файл программы поврежден!\nНекорректная шапка программы.",
        "Обаботка программы невозможна!\nВ программе используется более одного инструмента.",
        "Программа повреждена!\nНе найдено ни одного инструмента.",
        "Файл программы поврежден!\nНекорректная нумерация строк.",
        "Файл программы поврежден!\nНеожиданный конец программы.",
        "Ошибка поиска!\n'X' не найден.",
        "Ошибка поиска!\n'Y' не найден.",
        "Ошибка поиска!\n'Z' не найден.",
        "Ошибка поиска!\n'F' не найден.",
        "Ошибка поиска!\n'G' не найден."
    };
}

namespace FileOperations {
    const QString open = "Open";
    const QString save = "Save";
    const QString files = "files";
}
