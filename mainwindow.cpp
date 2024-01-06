#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  pDictionary = new Dictionary(this, Dictionary::Language::english);
  pSelectedFile = new SelectedFile(this);
  pGCode = new GCode(this);
  pOutForm = new OutForm(this);
  pAboutWindow = new About(this);
  QPixmap pixmap(":/picture.png");

  setWindowIcon(pixmap);
  ui->progressBar->setValue(0);
  ui->pb_loadFile->setEnabled(false);
  setEnabledWidgets(false);

  QObject::connect(ui->le_pathFile, &QLineEdit::textChanged, this,
                   [&] { pSelectedFile->exists(ui->le_pathFile->text()) ? ui->pb_loadFile->setEnabled(true) : ui->pb_loadFile->setEnabled(false); });
  QObject::connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::clearAll);
  QObject::connect(this, &MainWindow::sig_error, this, &MainWindow::rec_showMessageError);
  QObject::connect(pSelectedFile, &SelectedFile::sig_readError, this, &MainWindow::rec_showMessageError);
  QObject::connect(pGCode, &GCode::sig_errorNumeration, this, &MainWindow::rec_showMessageError);
  QObject::connect(this, &MainWindow::sig_errorFindValue, this, &MainWindow::rec_showMessageErrorFindValue);
  QObject::connect(this, &MainWindow::sig_warning, this, &MainWindow::rec_warning);
  QObject::connect(this, &MainWindow::sig_incrementProgressBar, this, [&] { ui->progressBar->setValue(ui->progressBar->value() + 1); });
  QObject::connect(this, &MainWindow::sig_readFile, this, &MainWindow::rec_readFile);
  QObject::connect(&ftrWtchTypeOfProcessingReady, &QFutureWatcher<bool>::finished, this, [&]() {
    qDebug() << "proc";
    emit sig_incrementProgressBar();
    rec_processingReady(ftrTypeOfProcessingReady.result());
  });
  QObject::connect(&ftrWtchSetToolReady, &QFutureWatcher<bool>::finished, this, [&]() {
    qDebug() << "tool";
    emit sig_incrementProgressBar();
    rec_processingReady(ftrSetToolReady.result());
  });
  QObject::connect(ui->spB_findFrame, &MySpinBox::sig_inFocus, this, [&]() {
    ui->statusbar->showMessage("Значение от " + QString::number(ui->spB_findFrame->minimum()) + " до " + QString::number(ui->spB_findFrame->maximum()));
  });
  QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_inFocus, this, [&]() {
    ui->statusbar->showMessage("Значение от " + QString::number(ui->spB_stopFrame->minimum()) + " до " + QString::number(ui->spB_stopFrame->maximum()));
  });
  QObject::connect(ui->spB_findFrame, &QSpinBox::editingFinished, this, [&]() { ui->statusbar->clearMessage(); });
  QObject::connect(ui->spB_stopFrame, &QSpinBox::editingFinished, this, [&]() { ui->statusbar->clearMessage(); });
  QObject::connect(ui->spB_findFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_findFrame_clicked);
  QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_calculate_clicked);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pb_findFile_clicked() {
  ui->le_pathFile->setText(
      QFileDialog::getOpenFileName(this, tr("%1").arg(pDictionary->translateString(FileOperations::open)), tr("%1").arg(GlobalVariables::homeDirOpenFile),
                                   tr("%1 %2 (*.%1)").arg(GlobalVariables::defaultFileFormat, pDictionary->translateString(FileOperations::files))));

  if (pSelectedFile->getFileName() != ui->le_pathFile->text()) {
    ui->pb_loadFile->setEnabled(true);
    clearAll();
  }
}

void MainWindow::on_pb_loadFile_clicked() {
  clearAll();
  setProgressText(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::readingFile]);
  emit sig_incrementProgressBar();

  pSelectedFile->setFileName(ui->le_pathFile->text());
  QFuture<QStringList> result = QtConcurrent::run([&] { return pSelectedFile->read(); });
  *pGCode = result.result();

  if (pGCode->isEmpty()) {
    setEnabledFileWidgets(true);
    emit sig_warning(Errors::Warning::wrnEmptyFile);
  } else {
    emit sig_readFile();
  }
}

void MainWindow::rec_readFile() {
  setProgressText(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::processingFile]);
  emit sig_incrementProgressBar();

  pGCode->checkCode();

  mListFile = pGCode->getProgramCode();

  setProgressText(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::searchingTypesOfProcessingAndUsedTools]);
  ftrTypeOfProcessingReady = QtConcurrent::run([&]() -> bool { return setTypeOfProcessing(); });
  ftrWtchTypeOfProcessingReady.setFuture(ftrTypeOfProcessingReady);
  ftrSetToolReady = QtConcurrent::run([&]() -> bool { return setTool(); });
  ftrWtchSetToolReady.setFuture(ftrSetToolReady);
}

void MainWindow::rec_processingReady(bool result) {
  static uint8_t m_countOperationsCompleted = 0;
  if (result) {
    ++m_countOperationsCompleted;
  }

  if (m_countOperationsCompleted == 2) {
    m_countOperationsCompleted = 0;
    displayDataToForm();
  } else {
    return;
  }
}

void MainWindow::displayDataToForm() {
  ui->lw_file->addItems(pGCode->getProgramCode());
  ui->lb_tool->setText('T' + m_tool);
  ui->lb_countOfFrames->setText(QString::number(pGCode->getCountOfFrames()));

  setRangeForStopAndFindSpinBoxes();
  setEnabledWidgets(true);
  setEnabledFileWidgets(true);

  setProgressText(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::finish]);
  ui->progressBar->setValue(ui->progressBar->maximum());
}

void MainWindow::setRangeForStopAndFindSpinBoxes() {
  uint32_t countOfFrames = ui->lb_countOfFrames->text().toUInt();
  if (countOfFrames - 2 >= 28) {
    ui->spB_stopFrame->setRange(20, countOfFrames - 2);
    ui->spB_stopFrame->setValue(20);
    ui->spB_findFrame->setRange(1, countOfFrames);
    ui->spB_findFrame->setValue(1);
    ui->statusbar->clearMessage();
  } else {
    ui->spB_stopFrame->setEnabled(false);
    emit sig_warning(Errors::Warning::wrnTooFewFrames);
  }
}

void MainWindow::on_pb_calculate_clicked() {
  uint32_t stopFrame = ui->spB_stopFrame->value();
  double X = 0, Y = 0, Z = 0;
  double F = 0;
  double G = 0;
  // double chekCommandValue = -100000;

  qDebug() << QString::fromStdString(std::to_string(stopFrame));

  ui->progressBar->setRange(0, 6);
  ui->progressBar->setValue(0);

  setProgressText("Ищу X, Y, Z, F, G...");
  if (findValue(X, stopFrame, 'X', true)) {
    emit sig_errorFindValue(Errors::erFindX);
    return;
  }
  if (findValue(Y, stopFrame, 'Y', true)) {
    emit sig_errorFindValue(Errors::erFindY);
    return;
  }
  if (findValue(Z, stopFrame, 'Z', false)) {
    emit sig_errorFindValue(Errors::erFindZ);
    return;
  }
  if (findValue(F, stopFrame, 'F', false)) {
    emit sig_errorFindValue(Errors::erFindF);
    return;
  }
  if (findValue(G, stopFrame, 'G', false)) {
    emit sig_errorFindValue(Errors::erFindG);
    return;
  }
  emit sig_incrementProgressBar();

  qDebug() << "Head = " + QString::number(pGCode->getHead());
  qDebug() << "X = " + QString::number(X);
  qDebug() << "Y = " + QString::number(Y);
  qDebug() << "Z = " + QString::number(Z);
  qDebug() << "F = " + QString::number(F);
  qDebug() << "G = " + QString::number(G);

  setProgressText("Генерация УП...");

  mListFileOut.clear();
  pOutForm->lwOutClear();

  for (uint32_t i = 0; i < pGCode->getHead(); ++i) {
    mListFileOut.push_back(mListFile[i]);
  }

  for (auto it = m_typesOfProcessing.end(); it != m_typesOfProcessing.begin();) {
    --it;
    if (it.key() < stopFrame) {
      qDebug() << QString::number(it.key());
      QString str = ";(";
      for (int j = 0; j < it.value().size() + 13; ++j) {
        str += "-";
      }
      str += ")";
      mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + ' ' + str + ' ');
      mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " ;(Processing - " + it.value() + ") ");
      mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + ' ' + str + ' ');

      break;
    }
  }

  if (G != 0) {
    mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G1 " + "X" + QString::number(X) + " Y" + QString::number(Y) + " F"
                           + QString::number(F) + ' ');
  } else {
    mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G0 " + "X" + QString::number(X) + " Y" + QString::number(Y) + ' ');
  }
  mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " Z" + QString::number(Z) + ' ');

  for (int i = stopFrame; i < mListFile.size(); ++i) {
    QString str = mListFile[i];
    if (i == mListFile.size() - 1) {
      mListFileOut.push_back(str);
    } else {
      str = deleteFrameNumber(str);
      mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + ' ' + str);
    }
  }

  pOutForm->lwOutAddList(mListFileOut);

  pOutForm->show();

  ui->lb_progress->setText("Готово!");
  ui->progressBar->setValue(ui->progressBar->maximum());
}

void MainWindow::on_pb_findFrame_clicked() {
  // ui->lw_file->setCurrentRow(0, QItemSelectionModel::SelectionFlag::SelectCurrent);
  ui->lw_file->setCurrentItem(ui->lw_file->item(ui->spB_findFrame->value()), QItemSelectionModel::SelectionFlag::Select);
  ui->lw_file->scrollToItem(ui->lw_file->item(ui->spB_findFrame->value()), QAbstractItemView::ScrollHint::PositionAtCenter);
}

void MainWindow::on_mb_help_about_triggered() { pAboutWindow->exec(); }

void MainWindow::on_mb_file_exit_triggered() { close(); }

void MainWindow::rec_showMessageError(Errors::Error er) {
  clearAll();
  setEnabledFileWidgets(true);
  pSelectedFile->clear();
  QMessageBox mBox;
  mBox.setWindowTitle(tr("%1").arg(pDictionary->translateString(Errors::errorMsg)));
  mBox.setIcon(QMessageBox::Icon::Critical);
  mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::errorDescription[er])));
  mBox.addButton(QMessageBox::Button::Ok);
  mBox.exec();
}

void MainWindow::rec_showMessageErrorFindValue(Errors::Error er) {
  pOutForm->close();
  pOutForm->lwOutClear();
  mListFileOut.clear();
  QMessageBox mBox;
  mBox.setWindowTitle(tr("%1").arg(pDictionary->translateString(Errors::errorMsg)));
  mBox.setIcon(QMessageBox::Icon::Critical);
  mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::errorDescription[er])));
  mBox.addButton(QMessageBox::Button::Ok);
  mBox.exec();
}

void MainWindow::rec_warning(Errors::Warning wrn) {
  QMessageBox mBox;
  mBox.setWindowTitle(tr("%1").arg(pDictionary->translateString(Errors::warningMsg)));
  mBox.setIcon(QMessageBox::Icon::Warning);
  mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::warningDescription[wrn])));
  mBox.addButton(QMessageBox::Button::Ok);
  mBox.exec();
}

void MainWindow::setEnabledFileWidgets(bool enabled) {
  ui->le_pathFile->setEnabled(enabled);
  ui->pb_loadFile->setEnabled(enabled);
  ui->pb_findFile->setEnabled(enabled);
  ui->menubar->setEnabled(enabled);
}

void MainWindow::setProgressText(QString text) {
  ui->lb_progress->setText(tr("%1").arg(text));
  while (ui->lb_progress->text() != text) {
    continue;
  }
}

bool MainWindow::setTypeOfProcessing() {
  m_typesOfProcessing = pGCode->getTypesOfProcessing();
  if (!m_typesOfProcessing.isEmpty()) {
    for (auto it = m_typesOfProcessing.constBegin(); it != m_typesOfProcessing.constEnd(); ++it) {
      ui->lw_typeOfProcessing->addItem(tr("N%1\t%2").arg(QString::number(it.key()), pDictionary->translateTypeOfProcessing(it.value())));
    }
  } else {
    emit sig_error(Errors::erNoTypeOfProcessing);
    return false;
  }
  return true;
}

bool MainWindow::setTool() {
  QStringList tools = pGCode->getTools();
  if (tools.isEmpty()) {
    emit sig_error(Errors::erNoTool);
    return false;
  }
  if (tools.size() > 1) {
    emit sig_error(Errors::erOneMoreTool);
    return false;
  }
  m_tool = tools[0];
  return true;
}

bool MainWindow::findValue(double &axe, int startFrame, QChar command, bool checkIJK) {
  axe = -99999;
  for (int i = startFrame; i > 0; --i) {
    QString str = mListFile[i];
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

void MainWindow::clearAll() {
  mListFile.clear();
  mListFileOut.clear();
  m_tool = "-";
  m_typesOfProcessing.clear();

  ui->lw_file->clear();
  ui->lw_typeOfProcessing->clear();
  ui->lb_tool->setText(m_tool);
  ui->lb_countOfFrames->setText("-");
  ui->lb_progress->setText("-");
  ui->spB_stopFrame->setValue(0);
  ui->spB_stopFrame->setRange(0, 0);
  ui->spB_stopFrame->setValue(0);
  ui->spB_findFrame->setRange(0, 0);
  ui->spB_findFrame->setValue(0);
  ui->progressBar->setRange(0, ProgressLoadingFile::progressOperationName.size() + 1);
  ui->progressBar->setValue(0);

  setEnabledWidgets(false);
}

QString MainWindow::deleteFrameNumber(QString str) {
  std::string strString = str.toStdString();
  auto it = std::find(strString.begin(), strString.end(), ' ');
  if (it != strString.end()) {
    while (*it == ' ') {
      ++it;
    }
    strString.erase(strString.begin(), it);
  }
  return QString::fromStdString(strString);
}

void MainWindow::setEnabledWidgets(bool enabled) {
  ui->grB_info->setEnabled(enabled);
  ui->grB_settings->setEnabled(enabled);
  ui->grB_findFrame->setEnabled(enabled);
  ui->pb_calculate->setEnabled(enabled);
  ui->pb_clear->setEnabled(enabled);
  ui->lw_file->setEnabled(enabled);
}
