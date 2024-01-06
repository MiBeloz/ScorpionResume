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
  QString getLanguageName();
  QString translateString(QString source);
  QString translateTypeOfProcessing(QString source);

private:
  Language m_language;
  const QVector<QString> m_languageName{"English", "Русский"};

  const QVector<QMap<QString, QString>> m_dictionaryString{
      {{"Open", "Open"}, {"Save", "Save"}, {"files", "files"}, {"Error!", "Error!"}, {"Warning!", "Warning!"}},
      {{"Open", "Открыть"}, {"Save", "Сохранить"}, {"files", "файлы"}, {"Error!", "Ошибка!"}, {"Warning!", "Предупреждение!"}}};

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
