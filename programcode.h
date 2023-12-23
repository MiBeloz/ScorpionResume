#ifndef PROGRAMCODE_H
#define PROGRAMCODE_H

#include <QObject>

class ProgramCode : public QObject {
    Q_OBJECT

public:
    explicit ProgramCode(QObject *parent = nullptr);

    bool isEmpty() const;

    ProgramCode& operator=(QStringList&& programCode);
    void toQStringList(QStringList &stringList);
private:
    QStringList m_progamCode;
};

#endif // PROGRAMCODE_H
