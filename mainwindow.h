#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QMap>
#include <QtConcurrent>
#include <QFileDialog>
#include <QString>
#include <QFile>
#include <QLabel>
#include <QProgressBar>

#include "data.h"
#include "outform.h"
#include "about.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sig_error(QString error);
    void sig_warning(QString caution);
    void sig_emptyFile();
    void sig_readyReadFile();
    void sig_incrementProgressBar();
    void sig_processingReady();

private slots:
    void on_pb_findFile_clicked();
    void on_pb_loadFile_clicked();
    void on_pb_calculate_clicked();
    void on_about_triggered();
    void rec_readyReadFile();
    void rec_processingReady();
    void rec_showMessageError(QString error);
    void rec_warning(QString caution);
    void rec_showMessageEmptyFile();

private:
    Ui::MainWindow *ui;
    OutForm *pOutForm;
    About *pAboutWindow;

    QString mPathFile;
    QStringList listFile;
    QString mTool;
    int16_t head = 0;
    QMap<int16_t, QString> typesOfProcessing;

    QLabel *pLabel;
    QProgressBar *pProgressBar;


    //QFuture<void> ftr;
    //QFutureWatcher<void> ftrWtch;

    void setEnabledWidgets(bool enabled);
    void setEnabledFileWidgets(bool enabled);
    bool setTypeOfProcessing();
    QString getStrOfProcessing(int i);
    QString getTypeOfProcessing(QString str);
    QString getFrameOfProcessing(QString str);
    bool setTool();
    QString getTool(int i);
    int findPosFrame(int frame);
    double findAxisValue(int posFrame, QChar axis, bool checkIJK);
    void clearAll();
    QString deleteFrameNumber(QString str);
};
#endif // MAINWINDOW_H
