#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QMap>
#include <QtConcurrent>
#include <QFileDialog>
#include <QString>

#include "data.h"
#include "selectedfile.h"
#include "gcode.h"
#include "outform.h"
#include "about.h"
#include "myspinbox.h"
#include "dictionary.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sig_error(Errors::Error);
    void sig_errorFindValue(Errors::Error);
    void sig_warning(QString caution);
    void sig_emptyFile();
    void sig_readFile();
    void sig_incrementProgressBar();
    void sig_processingReady();

private slots:
    void on_pb_findFile_clicked();
    void on_pb_loadFile_clicked();
    void rec_readFile();
    void rec_processingReady(bool result);
    void on_pb_calculate_clicked();
    void on_pb_findFrame_clicked();
    void on_mb_help_about_triggered();
    void on_mb_file_exit_triggered();
    void rec_showMessageError(Errors::Error er);
    void rec_showMessageErrorFindValue(Errors::Error er);
    void rec_warning(QString caution);
    void rec_showMessageEmptyFile();

private:
    Ui::MainWindow *ui;
    Dictionary *pDictionary;
    SelectedFile *pSelectedFile;
    GCode *pGCode;
    OutForm *pOutForm;
    About *pAboutWindow;

    QStringList mListFile;
    QStringList mListFileOut;
    QMap<uint32_t, QString> m_typesOfProcessing;
    QString m_tool;

    uint8_t m_countOperationsCompleted = 0;

    QFuture<bool> ftrTypeOfProcessingReady;
    QFuture<bool> ftrSetToolReady;
    QFutureWatcher<bool> ftrWtchTypeOfProcessingReady;
    QFutureWatcher<bool> ftrWtchSetToolReady;

    void setEnabledWidgets(bool enabled);
    void setEnabledFileWidgets(bool enabled);
    void setProgressText(QString text);
    bool setTypeOfProcessing();
    bool setTool();
    QString getTool(int i);
    int findPosFrame(int frame);
    double findValue(int posFrame, QChar axis, bool checkIJK);
    void clearAll();
    QString deleteFrameNumber(QString str);
};
#endif // MAINWINDOW_H
