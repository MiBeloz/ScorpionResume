#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QString>
#include <QFile>

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
    void sig_readyReadFile(QStringList strList);

private slots:
    void rec_readyReadFile(QStringList strList);
    void on_about_triggered();
    void on_pb_findFile_clicked();

    void on_pb_ok_clicked();

private:
    Ui::MainWindow *ui;
    About *pAboutWindow;
    QString mDir = "G:/";
    QFile mFile;
    QString mTool;
    int head = 0;

    void setEnabledWidgwts(bool enabled);
    void setTypeOfProcessing();
    void getTypeOfProcessing(QString type, int i);
    void setTool();
    void getTool(QString type, int i);
    void clearAll();
};
#endif // MAINWINDOW_H
