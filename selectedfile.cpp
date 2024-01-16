#include "selectedfile.h"

SelectedFile::SelectedFile(QObject *parent) : QObject(parent) {}

QString SelectedFile::getFileName() const {
  return m_file.fileName();
}

void SelectedFile::setFileName(QString fileName) {
  m_file.setFileName(fileName);
}

bool SelectedFile::exists(const QString &fileName) const {
  return m_file.exists(fileName);
}

QStringList SelectedFile::read() {
  m_ListFile.clear();
  if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit sig_readError(Errors::erOpen);
  }

  while (!m_file.atEnd()) {
    if (GlobalVariables::isUTF8Encoding) { // Если текст в локальной 8-бит кодировке (windows-1251);
      m_ListFile.append(QString::fromUtf8(m_file.readLine()));
    } else { // Если текст в UTF-8
      m_ListFile.append(QString::fromLocal8Bit(m_file.readLine()));
    }
  }
  m_file.close();

  return m_ListFile;
}

void SelectedFile::clear() {
  m_file.setFileName(QString{});
  m_ListFile.clear();
}
