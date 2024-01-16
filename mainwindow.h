#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>
#include <QMap>
#include <QMessageBox>
#include <QPixmap>
#include <QString>
#include <QtConcurrent>

#include "about.h"
#include "data.h"
#include "dictionary.h"
#include "gcode.h"
#include "myspinbox.h"
#include "outform.h"
#include "selectedfile.h"

QT_BEGIN_NAMESPACE
namespace Ui {
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

signals:
  void sig_error(Errors::Error, bool resetAll = true);
  void sig_warning(Errors::Warning wrn);
  void sig_readFile();
  void sig_incrementProgressBar();
  void sig_processingReady();

private slots:
  void on_pb_findFile_clicked();
  void on_pb_loadFile_clicked();
  void rec_readFile();
  bool checkTypeOfProcessing();
  bool checkTools();
  void rec_processingReady(bool result);
  void displayDataToForm();
  void setRangeForStopAndFindSpinBoxes();
  void on_pb_generate_clicked();
  void on_pb_findFrame_clicked();
  void on_mb_help_about_triggered();
  void on_mb_file_exit_triggered();
  void rec_showMessageError(Errors::Error er, bool resetAll = true);
  void rec_showMessageWarning(Errors::Warning wrn);

private:
  Ui::MainWindow *ui;
  Dictionary *pDictionary;
  SelectedFile *pSelectedFile;
  GCode *pGCode;
  OutForm *pOutForm;
  About *pAboutWindow;

  QFuture<bool> ftrTypeOfProcessingReady;
  QFuture<bool> ftrSetToolReady;
  QFutureWatcher<bool> ftrWtchTypeOfProcessingReady;
  QFutureWatcher<bool> ftrWtchSetToolReady;

  void changeLanguage(Dictionary::Language language);
  void setProgressText(QString text);
  void setEnabledWidgets(bool enabled);
  void setEnabledFileWidgets(bool enabled);
  void clearAll();
};
#endif // MAINWINDOW_H
