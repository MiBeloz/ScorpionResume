#ifndef PROGRAMCODE_H
#define PROGRAMCODE_H

#include <QObject>

#include "data.h"

class ProgramCode : public QObject {
    Q_OBJECT

public:
    explicit ProgramCode(QObject *parent = nullptr);

    bool isEmpty() const;

    ProgramCode& operator=(QStringList&& programCode);
    void checkCode();
    uint32_t getCountOfFrames();

    QStringList getProgramCode();

signals:
    void sig_errorNumeration(Errors::Error);

private:
    QStringList m_progamCode;
    uint32_t m_countOfFrames;

    void forEach(std::function<void(QString&)> f);
    void removeNewlines();
    void removeSpacesAndTabsFromBeginning();
    void calcCountOfFrames();
    void checkFrameNumber(QString &frame, uint32_t frameNumber);
};

#endif // PROGRAMCODE_H
