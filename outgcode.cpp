#include "outgcode.h"

OutGCode::OutGCode(QObject *parent) : QObject(parent) {

}

void OutGCode::reset() {
    m_GCodeOut.clear();
    m_outCodeReady = false;
    m_outX = 0;
    m_outY = 0;
    m_outZ = 0;
    m_outF = 0;
    m_outG = 0;
}

void OutGCode::generate(GCode* gcode, uint32_t stopFrame) {
    double X = 0, Y = 0, Z = 0;
    double F = 0;
    double G = 0;


    //setProgressText("Ищу X, Y, Z, F, G...");

    if (findValue(gcode, X, stopFrame, 'X', true)) {
        emit sig_errorFindValue(Errors::erFindX);
        return;
    }
    if (findValue(gcode, Y, stopFrame, 'Y', true)) {
        emit sig_errorFindValue(Errors::erFindY);
        return;
    }
    if (findValue(gcode, Z, stopFrame, 'Z', false)) {
        emit sig_errorFindValue(Errors::erFindZ);
        return;
    }
    if (findValue(gcode, F, stopFrame, 'F', false)) {
        emit sig_errorFindValue(Errors::erFindF);
        return;
    }
    if (findValue(gcode, G, stopFrame, 'G', false)) {
        emit sig_errorFindValue(Errors::erFindG);
        return;
    }
    emit sig_incrementProgressBar();

    qDebug() << "Head = " + QString::number(gcode->getHead());
    qDebug() << "X = " + QString::number(X);
    qDebug() << "Y = " + QString::number(Y);
    qDebug() << "Z = " + QString::number(Z);
    qDebug() << "F = " + QString::number(F);
    qDebug() << "G = " + QString::number(G);

    //setProgressText("Генерация УП...");

    m_GCodeOut.clear();

    for (uint32_t i = 0; i < gcode->getHead(); ++i) {
        m_GCodeOut.push_back(gcode->getProgramCode()[i]);
    }

    QMap<uint32_t, QString> typesOfProcessing = gcode->getTypesOfProcessing();
    for (auto it = typesOfProcessing.end(); it != typesOfProcessing.begin();) {
        --it;
        if (it.key() < stopFrame) {
            qDebug() << QString::number(it.key());
            QString str = ";(";
            for (int j = 0; j < it.value().size() + 13; ++j) {
                str += "-";
            }
            str += ")";
            m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + str + ' ');
            m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " ;(Processing - " + it.value() + ") ");
            m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + str + ' ');

            break;
        }
    }

    if (G != 0) {
        m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " G1 " + "X" + QString::number(X) + " Y" + QString::number(Y) + " F"
                             + QString::number(F) + ' ');
    } else {
        m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " G0 " + "X" + QString::number(X) + " Y" + QString::number(Y) + ' ');
    }
    m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + " Z" + QString::number(Z) + ' ');

    for (int i = stopFrame; i < gcode->getProgramCode().size(); ++i) {
        QString str = gcode->getProgramCode()[i];
        if (i == gcode->getProgramCode().size() - 1) {
            m_GCodeOut.push_back(str);
        } else {
            str = deleteFrameNumber(str);
            m_GCodeOut.push_back('N' + QString::number(m_GCodeOut.size()) + ' ' + str);
        }
    }
}

QStringList OutGCode::getProgramCode() {
    if (m_outCodeReady) {
        return m_GCodeOut;
    }
    return QStringList();
}

bool OutGCode::findValue(GCode* gcode, double &axe, int startFrame, QChar command, bool checkIJK) {
    axe = -99999;
    for (int i = startFrame; i > 0; --i) {
      QString str = gcode->getProgramCode()[i];
      bool strWithoutIJK = false;
      if (checkIJK) {
        qsizetype n = str.indexOf('I');
        if (n == -1) {
          n = str.indexOf('J');
          if (n == -1) {
            n = str.indexOf('K');
            if (n == -1) {
              strWithoutIJK = true;
            }
          }
        }
      } else {
        strWithoutIJK = true;
      }

      if (strWithoutIJK) {
        qsizetype n = str.lastIndexOf(command);
        if (n != -1) {
          str = str.mid(n + 1);

          int16_t m = 0;
          if (str[m] == '-') {
            ++m;
          }

          while (m < str.size() && (str[m].isDigit() || str[m] == '.')) {
            ++m;
          }
          str = str.left(m);

          axe = str.toDouble();
          break;
        }
      }
    }
    if (axe <= -99999) {
      return true;
    }
    return false;
}

QString OutGCode::deleteFrameNumber(QString frame) {
    std::string strString = frame.toStdString();
    auto it = std::find(strString.begin(), strString.end(), ' ');
    if (it != strString.end()) {
        while (*it == ' ') {
            ++it;
        }
        strString.erase(strString.begin(), it);
    }
    return QString::fromStdString(strString);
}
