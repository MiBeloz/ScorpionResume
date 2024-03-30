#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMap>
#include <QObject>
#include <QVector>

class Dictionary : public QObject {
  Q_OBJECT

public:
  enum Language { english, russian };

  enum Translate {
    Version,
    ProgramDescription,
    File,
    Exit,
    Help,
    About,
    Program,
    PathToTheProgram,
    Load,
    Find,
    Information,
    TypesOfProcessing,
    Tool,
    CountOfFrames,
    FrameSearch,
    Settings,
    StopFrame,
    Generate,
    ClearAll,
    ViewProgram,
    Progress,
    Result,
    Close,
    Open,
    Save,
    files,
    Error,
    Warning,
    Value,
    from,
    to,
    ReadingFile,
    ProcessingFile,
    LookingForTypesOfProcessingAndToolsUsed,
    LookingForValuesXYZFG,
    GeneratingTheCP,
    Ready,
    ErrorOpeningFile,
    ErrorEmptyFile,
    ErrorNoTypeOfProcessing,
    ErrorIncorrectHead,
    ErrorOneMoreTool,
    ErrorNoTool,
    ErrorIncorrectNumeration,
    ErrorEndProgram,
    ErrorFindX,
    ErrorFindY,
    ErrorFindZ,
    ErrorFindF,
    ErrorFindG,
    ErrorUnknown,
    WarningTooFewFrames
  };

  Dictionary(const Dictionary&) = delete;
  Dictionary& operator=(const Dictionary&) = delete;

  static Dictionary* init(QObject* parent = nullptr);

  void setLanguage(Language language);
  Dictionary::Language getLanguage();
  QString getLanguageName();

  QString translate(Dictionary::Translate source);
  QString translateTypeOfProcessing(QString source);

private:
  explicit Dictionary(QObject* parent);

  static Dictionary* mp_dictionary;
  static QMap<Dictionary::Language, QString> m_languageName;
  static QMap<Dictionary::Language, QMap<Dictionary::Translate, QString>> m_translate;
  Language m_eLanguage;

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
