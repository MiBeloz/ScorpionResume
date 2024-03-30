#include "dictionary.h"

Dictionary* Dictionary::mp_dictionary = nullptr;

QMap<Dictionary::Language, QString> Dictionary::m_languageName{
  {Dictionary::Language::english, "English"},
  {Dictionary::Language::russian, "Русский"}
};

QMap<Dictionary::Language, QMap<Dictionary::Translate, QString>> Dictionary::m_translate{
  {Dictionary::Language::english, {
    {Dictionary::Translate::Version, "Version"},
    {Dictionary::Translate::ProgramDescription, "\tDesigned to resume 'Scorpion' machine\n\tprograms from a specific frame"},
    {Dictionary::Translate::File, "File"},
    {Dictionary::Translate::Exit, "Exit"},
    {Dictionary::Translate::Help, "Help"},
    {Dictionary::Translate::About, "About"},
    {Dictionary::Translate::Program, "Program"},
    {Dictionary::Translate::PathToTheProgram, "Path to the program"},
    {Dictionary::Translate::Load, "Load"},
    {Dictionary::Translate::Find, "Find"},
    {Dictionary::Translate::Information, "Information"},
    {Dictionary::Translate::TypesOfProcessing, "Types of processing"},
    {Dictionary::Translate::Tool, "Tool"},
    {Dictionary::Translate::CountOfFrames, "Count of frames"},
    {Dictionary::Translate::FrameSearch, "Frame search"},
    {Dictionary::Translate::Settings, "Settings"},
    {Dictionary::Translate::StopFrame, "Stop frame"},
    {Dictionary::Translate::Generate, "Generate"},
    {Dictionary::Translate::ClearAll, "Clear all"},
    {Dictionary::Translate::ViewProgram, "View program"},
    {Dictionary::Translate::Progress, "Progress"},
    {Dictionary::Translate::Result, "Result"},
    {Dictionary::Translate::Close, "Close"},
    {Dictionary::Translate::Open, "Open"},
    {Dictionary::Translate::Save, "Save"},
    {Dictionary::Translate::files, "files"},
    {Dictionary::Translate::Error, "Error"},
    {Dictionary::Translate::Warning, "Warning"},
    {Dictionary::Translate::Value, "Value"},
    {Dictionary::Translate::from, "from"},
    {Dictionary::Translate::to, "to"},
    {Dictionary::Translate::ReadingFile, "Reading file"},
    {Dictionary::Translate::ProcessingFile, "Processing file"},
    {Dictionary::Translate::LookingForTypesOfProcessingAndToolsUsed, "Looking for types of processing and tools used"},
    {Dictionary::Translate::LookingForValuesXYZFG, "Looking for values X, Y, Z, F, G"},
    {Dictionary::Translate::GeneratingTheCP, "Generating the CP"},
    {Dictionary::Translate::Ready, "Ready"},
    {Dictionary::Translate::ErrorOpeningFile, "Error opening file!"},
    {Dictionary::Translate::ErrorEmptyFile, "The file cannot be read!\nEmpty file!"},
    {Dictionary::Translate::ErrorNoTypeOfProcessing, "The program is damaged!\nNo processing type found."},
    {Dictionary::Translate::ErrorIncorrectHead, "The program is damaged!\nIncorrect program header."},
    {Dictionary::Translate::ErrorOneMoreTool, "The program cannot be processed!\nThe program uses more than one tool."},
    {Dictionary::Translate::ErrorNoTool, "The program is corrupted!\nNo tools found."},
    {Dictionary::Translate::ErrorIncorrectNumeration, "The program is damaged!\nIncorrect line numbering."},
    {Dictionary::Translate::ErrorEndProgram, "The program is corrupted!\nUnexpected end of the program."},
    {Dictionary::Translate::ErrorFindX, "Search error!\n'X' not found."},
    {Dictionary::Translate::ErrorFindY, "Search error!\n'Y' not found."},
    {Dictionary::Translate::ErrorFindZ, "Search error!\n'Z' not found."},
    {Dictionary::Translate::ErrorFindF, "Search error!\n'F' not found."},
    {Dictionary::Translate::ErrorFindG, "Search error!\n'G' not found."},
    {Dictionary::Translate::ErrorUnknown, "Unknown error!"},
    {Dictionary::Translate::WarningTooFewFrames, "There are too few frames in the program\nto calculate the resumption of the program!"}},
  },
  {Dictionary::Language::russian, {
    {Dictionary::Translate::Version, "Версия"},
    {Dictionary::Translate::ProgramDescription, "\tПредназначена для возобновления программ\n\tстанка 'Scorpion' с определенного кадра"},
    {Dictionary::Translate::File, "Файл"},
    {Dictionary::Translate::Exit, "Выход"},
    {Dictionary::Translate::Help, "Справка"},
    {Dictionary::Translate::About, "О программе"},
    {Dictionary::Translate::Program, "Программа"},
    {Dictionary::Translate::PathToTheProgram, "Путь к программе"},
    {Dictionary::Translate::Load, "Загрузить"},
    {Dictionary::Translate::Find, "Найти"},
    {Dictionary::Translate::Information, "Информация"},
    {Dictionary::Translate::TypesOfProcessing, "Виды обработок"},
    {Dictionary::Translate::Tool, "Инструмент"},
    {Dictionary::Translate::CountOfFrames, "Количество кадров"},
    {Dictionary::Translate::FrameSearch, "Поиск кадра"},
    {Dictionary::Translate::Settings, "Установки"},
    {Dictionary::Translate::StopFrame, "Кадр останова"},
    {Dictionary::Translate::Generate, "Сгенерировать"},
    {Dictionary::Translate::ClearAll, "Очистить все"},
    {Dictionary::Translate::ViewProgram, "Просмотр программы"},
    {Dictionary::Translate::Progress, "Прогресс"},
    {Dictionary::Translate::Result, "Результат"},
    {Dictionary::Translate::Close, "Закрыть"},
    {Dictionary::Translate::Open, "Открыть"},
    {Dictionary::Translate::Save, "Сохранить"},
    {Dictionary::Translate::files, "файлы"},
    {Dictionary::Translate::Error, "Ошибка"},
    {Dictionary::Translate::Warning, "Предупреждение"},
    {Dictionary::Translate::Value, "Значение"},
    {Dictionary::Translate::from, "от"},
    {Dictionary::Translate::to, "до"},
    {Dictionary::Translate::ReadingFile, "Читаю файл"},
    {Dictionary::Translate::ProcessingFile, "Обрабатываю файл"},
    {Dictionary::Translate::LookingForTypesOfProcessingAndToolsUsed, "Ищу типы обработок и используемые инструменты"},
    {Dictionary::Translate::LookingForValuesXYZFG, "Ищу значения X, Y, Z, F, G"},
    {Dictionary::Translate::GeneratingTheCP, "Генерация УП"},
    {Dictionary::Translate::Ready, "Готово"},
    {Dictionary::Translate::ErrorOpeningFile, "Ошибка открытия файла"},
    {Dictionary::Translate::ErrorEmptyFile, "Чтение файла невозможно!\nПустой файл!"},
    {Dictionary::Translate::ErrorNoTypeOfProcessing, "Программа повреждена!\nНе найдено ни одного вида обработки."},
    {Dictionary::Translate::ErrorIncorrectHead, "Программа повреждена!\nНекорректная шапка программы."},
    {Dictionary::Translate::ErrorOneMoreTool, "Обаботка программы невозможна!\nВ программе используется более одного инструмента."},
    {Dictionary::Translate::ErrorNoTool, "Программа повреждена!\nНе найдено ни одного инструмента."},
    {Dictionary::Translate::ErrorIncorrectNumeration, "Программа повреждена!\nНекорректная нумерация строк."},
    {Dictionary::Translate::ErrorEndProgram, "Программа повреждена!\nНеожиданный конец программы."},
    {Dictionary::Translate::ErrorFindX, "Ошибка поиска!\n'X' не найден."},
    {Dictionary::Translate::ErrorFindY, "Ошибка поиска!\n'Y' не найден."},
    {Dictionary::Translate::ErrorFindZ, "Ошибка поиска!\n'Z' не найден."},
    {Dictionary::Translate::ErrorFindF, "Ошибка поиска!\n'F' не найден."},
    {Dictionary::Translate::ErrorFindG, "Ошибка поиска!\n'G' не найден."},
    {Dictionary::Translate::ErrorUnknown, "Неизветная ошибка!"},
    {Dictionary::Translate::WarningTooFewFrames, "В программе слишком мало кадров\nдля расчета возобновления программы!"}},
  }
};

Dictionary* Dictionary::init(QObject* parent) {
  if(!mp_dictionary) {
    mp_dictionary = new Dictionary(parent);
  }
  return mp_dictionary;
}

void Dictionary::setLanguage(Dictionary::Language language) {
  m_eLanguage = language;
}

Dictionary::Dictionary::Language Dictionary::getLanguage() {
  return m_eLanguage;
}

QString Dictionary::getLanguageName() {
  return m_languageName[m_eLanguage];
}

QString Dictionary::translate(Dictionary::Translate source) {
  return m_translate[m_eLanguage][source];
}

QString Dictionary::translateTypeOfProcessing(QString source) {
  auto it = m_dictionaryTypeOfProcessing[m_eLanguage].find(source);
  if (it != m_dictionaryTypeOfProcessing[m_eLanguage].end()) {
    return *it;
  }
  return source;
}

Dictionary::Dictionary(QObject* parent) : QObject(parent) {
  m_eLanguage = Dictionary::Language::english;
}
