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
#include "programcode.h"
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
    void sig_readyReadFile();
    void sig_incrementProgressBar();
    void sig_processingReady();

private slots:
    void on_pb_findFile_clicked();
    void on_pb_loadFile_clicked();
    void on_pb_calculate_clicked();
    void on_pb_findFrame_clicked();
    void on_mb_help_about_triggered();
    void on_mb_file_exit_triggered();
    void rec_readyReadFile();
    void rec_processingReady(bool result);
    void rec_showMessageError(Errors::Error er);
    void rec_showMessageErrorFindValue(Errors::Error er);
    void rec_warning(QString caution);
    void rec_showMessageEmptyFile();

private:
    Ui::MainWindow *ui;
    Dictionary *pDictionary;
    SelectedFile *pSelectedFile;
    ProgramCode *pProgramCode;
    OutForm *pOutForm;
    About *pAboutWindow;


    QStringList mListFile;
    QStringList mListFileOut;
    QString mTool;
    int16_t mHead = 0;
    QMap<size_t, QString> mTypesOfProcessing;
    size_t mCountOfFrames = 0;
    int mFlag = 0;

    QFuture<bool> ftrTypeOfProcessingReady;
    QFuture<bool> ftrSetToolReady;
    QFutureWatcher<bool> ftrWtchTypeOfProcessingReady;
    QFutureWatcher<bool> ftrWtchSetToolReady;

    void setEnabledWidgets(bool enabled);
    void setEnabledFileWidgets(bool enabled);
    bool setTypeOfProcessing();
    QString getStrOfProcessing(int i);
    QString getTypeOfProcessing(QString str);
    QString getFrameOfProcessing(QString str);
    bool setTool();
    QString getTool(int i);
    int findPosFrame(int frame);
    double findValue(int posFrame, QChar axis, bool checkIJK);
    void clearAll();
    QString deleteFrameNumber(QString str);
};
#endif // MAINWINDOW_H
