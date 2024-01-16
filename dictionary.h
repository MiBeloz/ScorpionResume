#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMap>
#include <QObject>
#include <QVector>

class Dictionary : public QObject {
  Q_OBJECT

public:
  enum Language { english = 0, russian };

  explicit Dictionary(QObject *parent = nullptr, Language language = Language::russian);

  void setLanguage(Language language);
  Language getLanguage();
  QString getLanguageName();
  QString translateString(QString source);
  QString translateTypeOfProcessing(QString source);

private:
  Language m_eLanguage;
  const QVector<QString> m_languageName{"English", "Русский"};

  const QVector<QMap<QString, QString>> m_dictionaryString{
      {{"Version", "Version"},
       {"Designed to resume 'Scorpion' machine programs from a specific frame", "Designed to resume 'Scorpion' machine\n\tprograms from a specific frame"},
       {"File", "File"},
       {"Exit", "Exit"},
       {"Help", "Help"},
       {"About", "About"},
       {"Program", "Program"},
       {"Path to the program", "Path to the program"},
       {"Load", "Load"},
       {"Find", "Find"},
       {"Information", "Information"},
       {"Types of processing", "Types of processing"},
       {"Tool", "Tool"},
       {"Count of frames", "Count of frames"},
       {"Frame search", "Frame search"},
       {"Settings", "Settings"},
       {"Stop frame", "Stop frame"},
       {"Generate", "Generate"},
       {"Clear all", "Clear all"},
       {"View program", "View program"},
       {"Progress", "Progress"},
       {"Result", "Result"},
       {"Close", "Close"},
       {"Open", "Open"},
       {"Save", "Save"},
       {"files", "files"},
       {"Error", "Error"},
       {"Warning", "Warning"},
       {"Value", "Value"},
       {"from", "from"},
       {"to", "to"},
       {"Reading file", "Reading file"},
       {"Processing file", "Processing file"},
       {"Looking for types of processing and tools used", "Looking for types of processing and tools used"},
       {"Looking for values X, Y, Z, F, G", "Looking for values X, Y, Z, F, G"},
       {"Generating the CP", "Generating the CP"},
       {"Ready", "Ready"}},

      {{"Version", "Версия"},
       {"Designed to resume 'Scorpion' machine programs from a specific frame",
        "Предназначена для возобновления программ\n\tстанка 'Scorpion' с определенного кадра"},
       {"File", "Файл"},
       {"Exit", "Выход"},
       {"Help", "Справка"},
       {"About", "О программе"},
       {"Program", "Программа"},
       {"Path to the program", "Путь к программе"},
       {"Load", "Загрузить"},
       {"Find", "Найти"},
       {"Information", "Информация"},
       {"Types of processing", "Виды обработок"},
       {"Tool", "Инструмент"},
       {"Count of frames", "Количество кадров"},
       {"Frame search", "Поиск кадра"},
       {"Settings", "Установки"},
       {"Stop frame", "Кадр останова"},
       {"Generate", "Сгенерировать"},
       {"Clear all", "Очистить все"},
       {"View program", "Просмотр программы"},
       {"Progress", "Прогресс"},
       {"Result", "Результат"},
       {"Close", "Закрыть"},
       {"Open", "Открыть"},
       {"Save", "Сохранить"},
       {"files", "файлы"},
       {"Error", "Ошибка"},
       {"Warning", "Предупреждение"},
       {"Value", "Значение"},
       {"from", "от"},
       {"to", "до"},
       {"Reading file", "Читаю файл"},
       {"Processing file", "Обрабатываю файл"},
       {"Looking for types of processing and tools used", "Ищу типы обработок и используемые инструменты"},
       {"Looking for values X, Y, Z, F, G", "Ищу значения X, Y, Z, F, G"},
       {"Generating the CP", "Генерация УП"},
       {"Ready", "Готово"}}};

  const QVector<QMap<QString, QString>> m_dictionaryTypeOfProcessing{
      {{"HSM-AREA-CLEARANCE", "(Ч)Контурная черновая"},
       {"HSM-RASTER-CLEARANCE", "(Ч)Растровая"},
       {"HSM-REST-ROUGHING", "(Ч)Черновая доработка"},
       {"HSM-BOUNDARY", "По границам"},
       {"HSM-COMBINE-CONST-Z-CONST-STEP", "Комплексная по ватерлиниям\n\tи с постоянным шагом"},
       {"HSM-COMBINE-CONST-Z-CORNER-OFFSET", "Комплексная по ватерлиниям\n\tи эквидистантный отступ"},
       {"HSM-COMBINE-CONST-Z-HORIZONTAL", "Комплексная по ватерлиниям\n\tи горизонтальная"},
       {"HSM-COMBINE-CONST-Z-LINEAR", "Комплексная по ватерлиниям\n\tи линейная"},
       {"HSM-CONST-STEP", "С постоянным шагом"},
       {"HSM-CORNER-OFFSET", "Эквидистантный отступ"},
       {"HSM-HELICAL", "Спирально-вертикальная"},
       {"HSM-HORIZ-AREA", "Горизонтальная"},
       {"HSM-MIXED-CONST-Z-CONST-STEP", "Призматическая деталь"},
       {"HSM-MORPHED", "Морфинг"},
       {"HSM-OFFSET-CUTTING", "Эквидистантная"},
       {"HSM-PAR-PENCIL", "Карандашная параллельная"},
       {"HSM-PENCIL", "Карандашная"},
       {"HSM-RADIAL", "Радиальная"},
       {"HSM-RASTER", "Линейная"},
       {"HSM-REST-MACH", "Чистовая доработка"},
       {"HSM-SPIRAL", "Спиральная"},
       {"HSM-WATERLINE", "По ватерлиниям"}},
      {{"HSM-AREA-CLEARANCE", "(Ч)Контурная черновая"},
       {"HSM-RASTER-CLEARANCE", "(Ч)Растровая"},
       {"HSM-REST-ROUGHING", "(Ч)Черновая доработка"},
       {"HSM-BOUNDARY", "По границам"},
       {"HSM-COMBINE-CONST-Z-CONST-STEP", "Комплексная по ватерлиниям\n\tи с постоянным шагом"},
       {"HSM-COMBINE-CONST-Z-CORNER-OFFSET", "Комплексная по ватерлиниям\n\tи эквидистантный отступ"},
       {"HSM-COMBINE-CONST-Z-HORIZONTAL", "Комплексная по ватерлиниям\n\tи горизонтальная"},
       {"HSM-COMBINE-CONST-Z-LINEAR", "Комплексная по ватерлиниям\n\tи линейная"},
       {"HSM-CONST-STEP", "С постоянным шагом"},
       {"HSM-CORNER-OFFSET", "Эквидистантный отступ"},
       {"HSM-HELICAL", "Спирально-вертикальная"},
       {"HSM-HORIZ-AREA", "Горизонтальная"},
       {"HSM-MIXED-CONST-Z-CONST-STEP", "Призматическая деталь"},
       {"HSM-MORPHED", "Морфинг"},
       {"HSM-OFFSET-CUTTING", "Эквидистантная"},
       {"HSM-PAR-PENCIL", "Карандашная параллельная"},
       {"HSM-PENCIL", "Карандашная"},
       {"HSM-RADIAL", "Радиальная"},
       {"HSM-RASTER", "Линейная"},
       {"HSM-REST-MACH", "Чистовая доработка"},
       {"HSM-SPIRAL", "Спиральная"},
       {"HSM-WATERLINE", "По ватерлиниям"}}};
};

#endif // DICTIONARY_H
