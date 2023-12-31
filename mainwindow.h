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
#include "outgcode.h"
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
  void sig_error(Errors::Error);
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
  void on_pb_calculate_clicked();
  void on_pb_findFrame_clicked();
  void on_mb_help_about_triggered();
  void on_mb_file_exit_triggered();
  void rec_showMessageError(Errors::Error er);
  void rec_showMessageErrorFindValue(Errors::Error er);
  void rec_warning(Errors::Warning wrn);

private:
  Ui::MainWindow *ui;
  Dictionary *pDictionary;
  SelectedFile *pSelectedFile;
  GCode *pGCode;
  OutGCode *pOutGCode;
  OutForm *pOutForm;
  About *pAboutWindow;

  //  QString m_tool;

  QFuture<bool> ftrTypeOfProcessingReady;
  QFuture<bool> ftrSetToolReady;
  QFutureWatcher<bool> ftrWtchTypeOfProcessingReady;
  QFutureWatcher<bool> ftrWtchSetToolReady;

  void setEnabledWidgets(bool enabled);
  void setEnabledFileWidgets(bool enabled);
  void setProgressText(QString text);
  void clearAll();
};
#endif // MAINWINDOW_H
