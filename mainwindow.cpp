#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  pDictionary = new Dictionary(this);
  changeLanguage(Dictionary::Language::russian);
  pSelectedFile = new SelectedFile(this);
  pGCode = new GCode(this);
  pOutForm = new OutForm(this);
  pAboutWindow = new About(this);
  QPixmap pixmap(":/picture.png");
  setWindowIcon(pixmap);

  QObject::connect(ui->le_pathFile, &QLineEdit::textChanged, this,
                   [&] { pSelectedFile->exists(ui->le_pathFile->text()) ? ui->pb_loadFile->setEnabled(true) : ui->pb_loadFile->setEnabled(false); });
  QObject::connect(ui->pb_clearAll, &QPushButton::clicked, this, &MainWindow::clearAll);
  QObject::connect(this, &MainWindow::sig_error, this, &MainWindow::rec_showMessageError);
  QObject::connect(pSelectedFile, &SelectedFile::sig_readError, this, &MainWindow::rec_showMessageError);
  QObject::connect(pGCode, &GCode::sig_error, this, &MainWindow::rec_showMessageError);
  QObject::connect(this, &MainWindow::sig_warning, this, &MainWindow::rec_showMessageWarning);
  QObject::connect(this, &MainWindow::sig_incrementProgressBar, this, [&] { ui->progressBar->setValue(ui->progressBar->value() + 1); });
  QObject::connect(this, &MainWindow::sig_readFile, this, &MainWindow::rec_readFile);
  QObject::connect(&ftrWtchTypeOfProcessingReady, &QFutureWatcher<bool>::finished, this, [&]() {
    emit sig_incrementProgressBar();
    rec_processingReady(ftrTypeOfProcessingReady.result());
  });
  QObject::connect(&ftrWtchSetToolReady, &QFutureWatcher<bool>::finished, this, [&]() {
    emit sig_incrementProgressBar();
    rec_processingReady(ftrSetToolReady.result());
  });
  QObject::connect(ui->spB_findFrame, &MySpinBox::sig_inFocus, this, [&]() {
    ui->statusbar->showMessage(tr("%1 %2 %3 %4 %5")
                                   .arg(pDictionary->translateString("Value"), pDictionary->translateString("from"),
                                        QString::number(ui->spB_findFrame->minimum()), pDictionary->translateString("to"),
                                        QString::number(ui->spB_findFrame->maximum())));
  });
  QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_inFocus, this, [&]() {
    ui->statusbar->showMessage(tr("%1 %2 %3 %4 %5")
                                   .arg(pDictionary->translateString("Value"), pDictionary->translateString("from"),
                                        QString::number(ui->spB_stopFrame->minimum()), pDictionary->translateString("to"),
                                        QString::number(ui->spB_stopFrame->maximum())));
  });
  QObject::connect(ui->spB_findFrame, &QSpinBox::editingFinished, this, [&]() { ui->statusbar->clearMessage(); });
  QObject::connect(ui->spB_stopFrame, &QSpinBox::editingFinished, this, [&]() { ui->statusbar->clearMessage(); });
  QObject::connect(ui->spB_findFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_findFrame_clicked);
  QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_generate_clicked);
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::on_pb_findFile_clicked() {
  ui->le_pathFile->setText(QFileDialog::getOpenFileName(this, tr("%1").arg(pDictionary->translateString("Open")),
                                                        tr("%1").arg(GlobalVariables::homeDirOpenFile),
                                                        tr("%1 %2 (*.%1)").arg(GlobalVariables::defaultFileFormat, pDictionary->translateString("files"))));

  if (pSelectedFile->getFileName() != ui->le_pathFile->text()) {
    ui->pb_loadFile->setEnabled(true);
    clearAll();
  }
}

void MainWindow::on_pb_loadFile_clicked() {
  clearAll();
  ui->progressBar->setRange(0, ProgressLoadingFile::progressOperation.size());
  setProgressText(pDictionary->translateString(ProgressLoadingFile::progressOperation[ProgressLoadingFile::readingFile]) + "...");
  emit sig_incrementProgressBar();

  pSelectedFile->setFileName(ui->le_pathFile->text());
  QStringList gcode = pSelectedFile->read();
  if (gcode.isEmpty()) {
    setEnabledFileWidgets(true);
    emit sig_error(Errors::Error::erEmptyFile);
  } else {
    if (pGCode->addGCode(gcode)) {
      emit sig_readFile();
    }
  }
}

void MainWindow::rec_readFile() {
  setProgressText(pDictionary->translateString(ProgressLoadingFile::progressOperation[ProgressLoadingFile::processingFile]) + "...");
  emit sig_incrementProgressBar();

  setProgressText(pDictionary->translateString(ProgressLoadingFile::progressOperation[ProgressLoadingFile::searchingTypesOfProcessingAndUsedTools]) + "...");
  ftrTypeOfProcessingReady = QtConcurrent::run([&]() -> bool { return checkTypeOfProcessing(); });
  ftrWtchTypeOfProcessingReady.setFuture(ftrTypeOfProcessingReady);
  ftrSetToolReady = QtConcurrent::run([&]() -> bool { return checkTools(); });
  ftrWtchSetToolReady.setFuture(ftrSetToolReady);
}

bool MainWindow::checkTypeOfProcessing() {
  QMap<int, QString> typesOfProcessing = pGCode->getTypesOfProcessing();
  if (typesOfProcessing.isEmpty()) {
    pGCode->reset();
    emit sig_error(Errors::erNoTypeOfProcessing);
    return false;
  }
  return true;
}

bool MainWindow::checkTools() {
  QStringList tools = pGCode->getTools();
  if (tools.isEmpty()) {
    pGCode->reset();
    emit sig_error(Errors::erNoTool);
    return false;
  }
  if (tools.size() > 1) {
    pGCode->reset();
    emit sig_error(Errors::erOneMoreTool);
    return false;
  }
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
  QMap<int, QString> typesOfProcessing = pGCode->getTypesOfProcessing();
  for (auto it = typesOfProcessing.constBegin(); it != typesOfProcessing.constEnd(); ++it) {
    ui->lw_typeOfProcessing->addItem(tr("N%1\t%2").arg(QString::number(it.key()), pDictionary->translateTypeOfProcessing(it.value())));
  }
  QStringList tools = pGCode->getTools();
  for (const QString &tool : tools) {
    if (ui->lb_tool->text() == '-') {
      ui->lb_tool->setText('T' + tool + ' ');
    } else {
      ui->lb_tool->setText(ui->lb_tool->text() + 'T' + tool + ' ');
    }
  }
  ui->lb_countOfFrames->setText(QString::number(pGCode->getCountOfFrames()));

  setRangeForStopAndFindSpinBoxes();
  setEnabledWidgets(true);
  setEnabledFileWidgets(true);

  setProgressText(pDictionary->translateString(ProgressLoadingFile::progressOperation[ProgressLoadingFile::finish]) + '!');
  ui->progressBar->setValue(ui->progressBar->maximum());
}

void MainWindow::setRangeForStopAndFindSpinBoxes() {
  int countOfFrames = pGCode->getCountOfFrames();
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

void MainWindow::on_pb_generate_clicked() {
  ui->progressBar->setRange(0, ProgressGenerateFile::progressGenerate.size());
  ui->progressBar->reset();

  setProgressText(pDictionary->translateString(ProgressGenerateFile::progressGenerate[ProgressGenerateFile::OperationName::searchXYZFG]) + "...");
  emit sig_incrementProgressBar();
  pGCode->generateOutProgramCode(pGCode, ui->spB_stopFrame->value());

  setProgressText(pDictionary->translateString(ProgressGenerateFile::progressGenerate[ProgressGenerateFile::OperationName::generate]) + "...");
  emit sig_incrementProgressBar();
  pOutForm->lwOutClear();
  QStringList outCode = pGCode->getOutProgramCode();
  if (!outCode.isEmpty()) {
    pOutForm->lwOutAddList(outCode);
    pOutForm->show();
    pOutForm->setWindowTitle(tr("%1").arg(pDictionary->translateString("Result")));
    pOutForm->changeLanguage(pDictionary->getLanguage());
  }

  setProgressText(pDictionary->translateString(ProgressGenerateFile::progressGenerate[ProgressGenerateFile::OperationName::finish] + '!'));
  ui->progressBar->setValue(ui->progressBar->maximum());
}

void MainWindow::on_pb_findFrame_clicked() {
  // ui->lw_file->setCurrentRow(0, QItemSelectionModel::SelectionFlag::SelectCurrent);
  ui->lw_file->setCurrentItem(ui->lw_file->item(ui->spB_findFrame->value()), QItemSelectionModel::SelectionFlag::Select);
  ui->lw_file->scrollToItem(ui->lw_file->item(ui->spB_findFrame->value()), QAbstractItemView::ScrollHint::PositionAtCenter);
}

void MainWindow::on_mb_help_about_triggered() {
  pAboutWindow->setModal(true);
  pAboutWindow->show();
  pAboutWindow->setWindowTitle(tr("%1").arg(pDictionary->translateString("About")));
  pAboutWindow->changeLanguage(pDictionary->getLanguage());
}

void MainWindow::on_mb_file_exit_triggered() {
  close();
}

void MainWindow::rec_showMessageError(Errors::Error er, bool resetAll) {
  if (resetAll) {
    clearAll();
    setEnabledFileWidgets(true);
    pSelectedFile->clear();
  }

  pOutForm->close();
  pOutForm->lwOutClear();
  QMessageBox mBox;
  mBox.setWindowTitle(tr("%1!").arg(pDictionary->translateString(Errors::errorMsg)));
  mBox.setIcon(QMessageBox::Icon::Critical);
  mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::errorDescription[er])));
  mBox.addButton(QMessageBox::Button::Ok);
  mBox.exec();
}

void MainWindow::rec_showMessageWarning(Errors::Warning wrn) {
  QMessageBox mBox;
  mBox.setWindowTitle(tr("%1!").arg(pDictionary->translateString(Errors::warningMsg)));
  mBox.setIcon(QMessageBox::Icon::Warning);
  mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::warningDescription[wrn])));
  mBox.addButton(QMessageBox::Button::Ok);
  mBox.exec();
}

void MainWindow::changeLanguage(Dictionary::Language language) {
  pDictionary->setLanguage(language);
  ui->mb_file->setTitle(tr("%1").arg(pDictionary->translateString("File")));
  ui->mb_file_exit->setText(tr("%1").arg(pDictionary->translateString("Exit")));
  ui->mb_help->setTitle(tr("%1").arg(pDictionary->translateString("Help")));
  ui->mb_help_about->setText(tr("%1").arg(pDictionary->translateString("About")));
  ui->grB_program->setTitle(tr("%1").arg(pDictionary->translateString("Program")));
  ui->lb_progPath->setText(tr("%1:").arg(pDictionary->translateString("Path to the program")));
  ui->pb_loadFile->setText(tr("%1").arg(pDictionary->translateString("Load")));
  ui->pb_findFile->setText(tr("%1").arg(pDictionary->translateString("Find")));
  ui->grB_info->setTitle(tr("%1").arg(pDictionary->translateString("Information")));
  ui->grB_typeOfProcessing->setTitle(tr("%1").arg(pDictionary->translateString("Types of processing")));
  ui->lb_toolName->setText(tr("%1:").arg(pDictionary->translateString("Tool")));
  ui->lb_countOfFramesName->setText(tr("%1:").arg(pDictionary->translateString("Count of frames")));
  ui->grB_findFrame->setTitle(tr("%1").arg(pDictionary->translateString("Frame search")));
  ui->pb_findFrame->setText(tr("%1").arg(pDictionary->translateString("Find")));
  ui->grB_settings->setTitle(tr("%1").arg(pDictionary->translateString("Settings")));
  ui->lb_stopFrame->setText(tr("%1:").arg(pDictionary->translateString("Stop frame")));
  ui->pb_generate->setText(tr("%1").arg(pDictionary->translateString("Generate")));
  ui->pb_clearAll->setText(tr("%1").arg(pDictionary->translateString("Clear all")));
  ui->grB_viewProgram->setTitle(tr("%1").arg(pDictionary->translateString("View program")));
  ui->grB_progress->setTitle(tr("%1").arg(pDictionary->translateString("Progress")));
}

void MainWindow::setProgressText(QString text) {
  ui->lb_progress->setText(tr("%1").arg(text));
  while (ui->lb_progress->text() != text) {
    continue;
  }
}

void MainWindow::setEnabledWidgets(bool enabled) {
  ui->grB_info->setEnabled(enabled);
  ui->grB_settings->setEnabled(enabled);
  ui->grB_findFrame->setEnabled(enabled);
  ui->pb_generate->setEnabled(enabled);
  ui->pb_clearAll->setEnabled(enabled);
  ui->lw_file->setEnabled(enabled);
}

void MainWindow::setEnabledFileWidgets(bool enabled) {
  ui->le_pathFile->setEnabled(enabled);
  ui->pb_loadFile->setEnabled(enabled);
  ui->pb_findFile->setEnabled(enabled);
  ui->menubar->setEnabled(enabled);
}

void MainWindow::clearAll() {
  pGCode->reset();
  ui->lw_file->clear();
  ui->lw_typeOfProcessing->clear();
  ui->lb_tool->setText(tr("%1").arg("-"));
  ui->lb_countOfFrames->setText(tr("%1").arg("-"));
  ui->lb_progress->setText(tr("%1").arg("-"));
  ui->spB_stopFrame->setValue(0);
  ui->spB_stopFrame->setRange(0, 0);
  ui->spB_stopFrame->setValue(0);
  ui->spB_findFrame->setRange(0, 0);
  ui->spB_findFrame->setValue(0);
  ui->progressBar->reset();

  setEnabledWidgets(false);
}
