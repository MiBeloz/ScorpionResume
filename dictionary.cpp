#include "dictionary.h"

Dictionary::Dictionary(QObject *parent, Dictionary::Language language) : QObject{parent} { m_language = language; }

void Dictionary::setLanguage(Dictionary::Language language) { m_language = language; }

QString Dictionary::getLanguageName() { return m_languageName[m_language]; }

QString Dictionary::translateString(QString source) {
  auto it = m_dictionaryString[m_language].find(source);
  if (it != m_dictionaryString[m_language].end()) {
    return *it;
  }
  return source;
}

QString Dictionary::translateTypeOfProcessing(QString source) {
  auto it = m_dictionaryTypeOfProcessing[m_language].find(source);
  if (it != m_dictionaryTypeOfProcessing[m_language].end()) {
    return *it;
  }
  return source;
}
