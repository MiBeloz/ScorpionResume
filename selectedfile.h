#ifndef SELECTEDFILE_H
#define SELECTEDFILE_H

#include <QObject>
#include <QFile>

class SelectedFile : public QObject {
    Q_OBJECT

public:
    SelectedFile(QObject *parent = nullptr);

    QString getPathFile();
    void setPathFile(QString pathFile);
    void clear();

private:
    QFile m_file;
    QString m_pathFile;
};

#endif // SELECTFILE_H
