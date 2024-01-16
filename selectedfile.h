#ifndef SELECTEDFILE_H
#define SELECTEDFILE_H

#include <QFile>
#include <QObject>

#include "data.h"

class SelectedFile : public QObject {
  Q_OBJECT

public:
  explicit SelectedFile(QObject *parent = nullptr);

  QString getFileName() const;
  void setFileName(QString fileName);
  bool exists(const QString &fileName) const;
  QStringList read();
  void clear();

signals:
  void sig_readError(Errors::Error, bool resetAll = true);

private:
  QFile m_file;
  QStringList m_ListFile;
};

#endif // SELECTFILE_H
