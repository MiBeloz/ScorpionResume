#include "selectedfile.h"

SelectedFile::SelectedFile(QObject *parent) : QObject(parent) {}

QString SelectedFile::getFileName() const {
  return m_file.fileName();
}

void SelectedFile::setFileName(QString fileName) {
  m_file.setFileName(fileName);
}

bool SelectedFile::exists(const QString &fileName) {
  return m_file.exists(fileName);
}

QStringList SelectedFile::read() {
  QStringList result;
  if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    emit sig_readError(Errors::erOpen);
  }

  while (!m_file.atEnd()) {
    if (GlobalVariables::isUTF8Encoding) { // Если текст в локальной 8-бит кодировке (windows-1251);
      result.append(QString::fromUtf8(m_file.readLine()));
    } else { // Если текст в UTF-8
      result.append(QString::fromLocal8Bit(m_file.readLine()));
    }
  }
  m_file.close();

  return result;
}

void SelectedFile::clear() {
  m_file.setFileName(QString{});
}
