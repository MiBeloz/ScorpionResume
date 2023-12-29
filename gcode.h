#ifndef GCODE_H
#define GCODE_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QSet>
#include <regex>

#include "data.h"

class GCode : public QObject {
    Q_OBJECT

public:
    explicit GCode(QObject *parent = nullptr);

    GCode(GCode& other) = delete;
    GCode(GCode&& other) = delete;
    GCode& operator=(QStringList& GCode) = delete;
    GCode& operator=(QStringList&& GCode);

    bool isEmpty() const;
    void checkCode();
    uint32_t getCountOfFrames();
    uint32_t getHead();
    QMap<uint32_t, QString> getTypesOfProcessing();
    QStringList getTools();

    QStringList getProgramCode();

signals:
    void sig_errorNumeration(Errors::Error);

private:
    QStringList m_GCode;
    uint32_t m_countOfFrames;
    uint32_t m_countHeadFrames;
    QMap<uint32_t, QString> m_typesOfProcessing;
    QSet<QString> m_tools;


    void reset();
    void forEach(std::function<void(QString&)> f);
    void removeNewlines();
    void removeSpacesAndTabsFromBeginning();
    void calcCountOfFrames();
    bool checkFrameNumber(QString &frame, uint32_t frameNumber);
    bool frameIsProcessingName(QString frame);
    QString getTypeOfProcessing(QString frame);
    uint32_t getFrameNumber(QString frame);
    bool frameIsTool(QString frame);
    QString getTool(QString frame);
};

#endif // GCODE_H
