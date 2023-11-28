#include "data.h"

QString versionMajor = "2";
QString versionMinor = "1";
QString versionMicro = "2";

QString gDir {"G:/"};
bool gUTF8 {false};

QMap<QString, QString> dictionary {
    { "HSM-AREA-CLEARANCE", "(Ч)Контурная черновая" },
    { "HSM-RASTER-CLEARANCE", "(Ч)Растровая" },
    { "HSM-REST-ROUGHING", "(Ч)Черновая доработка" },
    { "HSM-BOUNDARY", "По границам" },
    { "HSM-COMBINE-CONST-Z-CONST-STEP", "Комплексная по ватерлиниям\n\tи с постоянным шагом" },
    { "HSM-COMBINE-CONST-Z-CORNER-OFFSET", "Комплексная по ватерлиниям\n\tи эквидистантный отступ" },
    { "HSM-COMBINE-CONST-Z-HORIZONTAL", "Комплексная по ватерлиниям\n\tи горизонтальная" },
    { "HSM-COMBINE-CONST-Z-LINEAR", "Комплексная по ватерлиниям\n\tи линейная" },
    { "HSM-CONST-STEP", "С постоянным шагом" },
    { "HSM-CORNER-OFFSET", "Эквидистантный отступ" },
    { "HSM-HELICAL", "Спирально-вертикальная" },
    { "HSM-HORIZ-AREA", "Горизонтальная" },
    { "HSM-MIXED-CONST-Z-CONST-STEP", "Призматическая деталь" },
    { "HSM-MORPHED", "Морфинг" },
    { "HSM-OFFSET-CUTTING", "Эквидистантная" },
    { "HSM-PAR-PENCIL", "Карандашная параллельная" },
    { "HSM-PENCIL", "Карандашная" },
    { "HSM-RADIAL", "Радиальная" },
    { "HSM-RASTER", "Линейная" },
    { "HSM-REST-MACH", "Чистовая доработка" },
    { "HSM-SPIRAL", "Спиральная" },
    { "HSM-WATERLINE", "По ватерлиниям" } };
