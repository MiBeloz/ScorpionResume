#include "data.h"

namespace ApplicationVersion {
  const QString versionMajor = "2";
  const QString versionMinor = "2";
  const QString versionMicro = "13";
} // namespace ApplicationVersion

namespace GlobalVariables {
  QString homeDirOpenFile = "G:/";
  bool isUTF8Encoding = false;
  QString defaultFileFormat = "MPF";
} // namespace GlobalVariables

namespace ProgressLoadingFile {
  const QVector<QString> progressOperation{"Reading file", "Processing file", "Looking for types of processing and tools used", "Ready"};
}

namespace ProgressGenerateFile {
  const QVector<QString> progressGenerate{"Looking for values X, Y, Z, F, G", "Generating the CP", "Ready"};
}

namespace Errors {
  const QString errorMsg = "Error";
  const QString warningMsg = "Warning";
  const QVector<QString> errorDescription{"Ошибка открытия файла!",
                                          "Чтение файла невозможно!\nПустой файл!",
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
                                          "Ошибка поиска!\n'G' не найден.",
                                          "Ошибка конечного файла программы!"};
  const QVector<QString> warningDescription{"В программе слишком мало кадров\nдля расчета возобновления программы!"};
} // namespace Errors
