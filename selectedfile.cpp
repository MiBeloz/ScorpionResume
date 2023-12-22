#include "selectedfile.h"

SelectedFile::SelectedFile(QObject *parent) : QObject(parent) {}

QString SelectedFile::getPathFile() {
    return m_pathFile;
}

void SelectedFile::setPathFile(QString pathFile) {
    m_pathFile = pathFile;
}

void SelectedFile::clear() {
    m_pathFile.clear();
}
