#include "dictionary.h"

Dictionary::Dictionary(QObject *parent, Dictionary::Language language) : QObject{parent} {
  m_eLanguage = language;
}

void Dictionary::setLanguage(Dictionary::Language language) {
  m_eLanguage = language;
}

Dictionary::Language Dictionary::getLanguage() {
  return m_eLanguage;
}

QString Dictionary::getLanguageName() {
  return m_languageName[m_eLanguage];
}

QString Dictionary::translateString(QString source) {
  auto it = m_dictionaryString[m_eLanguage].find(source);
  if (it != m_dictionaryString[m_eLanguage].end()) {
    return *it;
  }
  return source;
}

QString Dictionary::translateTypeOfProcessing(QString source) {
  auto it = m_dictionaryTypeOfProcessing[m_eLanguage].find(source);
  if (it != m_dictionaryTypeOfProcessing[m_eLanguage].end()) {
    return *it;
  }
  return source;
}
