#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  pDictionary = new Dictionary(this, Dictionary::Language::english);
  pSelectedFile = new SelectedFile(this);
  pGCode = new GCode(this);
  pOutGCode = new OutGCode(this);
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
  QObject::connect(pOutGCode, &OutGCode::sig_errorFindValue, this, &MainWindow::rec_showMessageErrorFindValue);
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
    ui->progressBar->setRange(0, 6);
    ui->progressBar->setValue(0);

    pOutGCode->generate(pGCode, ui->spB_stopFrame->value());
    pOutForm->lwOutClear();
    QStringList outCode = pOutGCode->getProgramCode();
    if (!outCode.isEmpty()) {
        pOutForm->lwOutAddList(outCode);
    }
    else {
        emit sig_error(Errors::Error::erOutFile);
    }
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

void MainWindow::setEnabledWidgets(bool enabled) {
  ui->grB_info->setEnabled(enabled);
  ui->grB_settings->setEnabled(enabled);
  ui->grB_findFrame->setEnabled(enabled);
  ui->pb_calculate->setEnabled(enabled);
  ui->pb_clear->setEnabled(enabled);
  ui->lw_file->setEnabled(enabled);
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
