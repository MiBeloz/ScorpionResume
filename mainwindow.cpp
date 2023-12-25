#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    pDictionary = new Dictionary(this, Dictionary::Language::english);
    pSelectedFile = new SelectedFile(this);
    pProgramCode = new ProgramCode(this);
    pOutForm = new OutForm(this);
    pAboutWindow = new About(this);
    QPixmap pixmap(":/picture.png");
    setWindowIcon(pixmap);

    ui->progressBar->setValue(0);
    ui->pb_loadFile->setEnabled(false);
    setEnabledWidgets(false);
    QObject::connect(ui->le_pathFile, &QLineEdit::textChanged, this, [&]{
        pSelectedFile->exists(ui->le_pathFile->text()) ? ui->pb_loadFile->setEnabled(true) : ui->pb_loadFile->setEnabled(false); });

    QObject::connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::clearAll);
    QObject::connect(this, &MainWindow::sig_error, this, &MainWindow::rec_showMessageError);
    QObject::connect(pSelectedFile, &SelectedFile::sig_readError, this, &MainWindow::rec_showMessageError);
    QObject::connect(pProgramCode, &ProgramCode::sig_errorNumeration, this, &MainWindow::rec_showMessageError);
    QObject::connect(this, &MainWindow::sig_errorFindValue, this, &MainWindow::rec_showMessageErrorFindValue);
    QObject::connect(this, &MainWindow::sig_warning, this, &MainWindow::rec_warning);
    QObject::connect(this, &MainWindow::sig_emptyFile, this, &MainWindow::rec_showMessageEmptyFile);
    QObject::connect(this, &MainWindow::sig_incrementProgressBar, this, [&]{ ui->progressBar->setValue(ui->progressBar->value() + 1); });
    QObject::connect(this, &MainWindow::sig_readyReadFile, this, &MainWindow::rec_readyReadFile);
    QObject::connect(&ftrWtchTypeOfProcessingReady, &QFutureWatcher<bool>::finished, this, [&](){ rec_processingReady(ftrTypeOfProcessingReady.result()); });
    QObject::connect(&ftrWtchSetToolReady, &QFutureWatcher<bool>::finished, this, [&](){ rec_processingReady(ftrSetToolReady.result()); });
    QObject::connect(ui->spB_findFrame, &MySpinBox::sig_inFocus, this, [&](){
        ui->statusbar->showMessage("Значение от " + QString::number(ui->spB_findFrame->minimum()) + " до " + QString::number(ui->spB_findFrame->maximum()));
    });
    QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_inFocus, this, [&](){
        ui->statusbar->showMessage("Значение от " + QString::number(ui->spB_stopFrame->minimum()) + " до " + QString::number(ui->spB_stopFrame->maximum()));
    });
    QObject::connect(ui->spB_findFrame, &QSpinBox::editingFinished, this, [&](){
        ui->statusbar->clearMessage();
    });
    QObject::connect(ui->spB_stopFrame, &QSpinBox::editingFinished, this, [&](){
        ui->statusbar->clearMessage();
    });
    QObject::connect(ui->spB_findFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_findFrame_clicked);
    QObject::connect(ui->spB_stopFrame, &MySpinBox::sig_enterPress, this, &MainWindow::on_pb_calculate_clicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pb_findFile_clicked() {
    ui->le_pathFile->setText(QFileDialog::getOpenFileName(this,
                                                          tr("%1").arg(pDictionary->translateString(FileOperations::open)),
                                                          tr("%1").arg(GlobalVariables::homeDirOpenFile),
                                                          tr("%1 %2 (*.%1)").arg(GlobalVariables::defaultFileFormat, pDictionary->translateString(FileOperations::files))));

    if (pSelectedFile->getFileName() != ui->le_pathFile->text()) {
        ui->pb_loadFile->setEnabled(true);
        clearAll();
    }
}

void MainWindow::on_pb_loadFile_clicked() {
    clearAll();
    ui->lb_progress->setText(tr("%1").arg(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::readingFile]));

    pSelectedFile->setFileName(ui->le_pathFile->text());
    QFuture<QStringList> result = QtConcurrent::run([&] { return pSelectedFile->read(); });
    *pProgramCode = result.result();

    if (pProgramCode->isEmpty()) {
        setEnabledFileWidgets(true);
        emit sig_emptyFile();
    }
    else {
        emit sig_readyReadFile();
    }
}

void MainWindow::rec_readyReadFile() {
    ui->lb_progress->setText(ProgressLoadingFile::progressOperationName[ProgressLoadingFile::processingFile]);
    emit sig_incrementProgressBar();

    pProgramCode->checkCode();
    mListFile = pProgramCode->getProgramCode();

    emit sig_incrementProgressBar();

    ftrTypeOfProcessingReady = QtConcurrent::run([&]() -> bool { return setTypeOfProcessing(); });
    ftrWtchTypeOfProcessingReady.setFuture(ftrTypeOfProcessingReady);
    ftrSetToolReady = QtConcurrent::run([&]() -> bool { return setTool(); });
    ftrWtchSetToolReady.setFuture(ftrSetToolReady);
}

void MainWindow::on_pb_calculate_clicked() {
    size_t frame = ui->spB_stopFrame->value();
    double X = 0, Y = 0, Z = 50;
    double F = 0;
    int G = 0;
    double chekCommandValue = -100000;

    qDebug() << QString::fromStdString(std::to_string(frame));

    ui->progressBar->setRange(0, 6);
    ui->progressBar->setValue(0);

    ui->lb_progress->setText("Ищу X, Y, Z, F, G...");
    X = findValue(frame, 'X', true);
    if (X < chekCommandValue) {
        emit sig_errorFindValue(Errors::erFindX);
        return;
    }
    emit sig_incrementProgressBar();

    Y = findValue(frame, 'Y', true);
    if (Y < chekCommandValue) {
        emit sig_errorFindValue(Errors::erFindY);
        return;
    }
    emit sig_incrementProgressBar();

    Z = findValue(frame, 'Z', false);
    if (Z < chekCommandValue) {
        emit sig_errorFindValue(Errors::erFindZ);
        return;
    }
    emit sig_incrementProgressBar();

    F = findValue(frame, 'F', false);
    if (F < chekCommandValue) {
        emit sig_errorFindValue(Errors::erFindF);
        return;
    }
    emit sig_incrementProgressBar();

    G = findValue(frame, 'G', false);
    if (G < chekCommandValue) {
        emit sig_errorFindValue(Errors::erFindG);
        return;
    }
    emit sig_incrementProgressBar();

    qDebug() << "Head = " + QString::number(mHead);
    qDebug() << "X = " + QString::number(X);
    qDebug() << "Y = " + QString::number(Y);
    qDebug() << "Z = " + QString::number(Z);
    qDebug() << "F = " + QString::number(F);
    qDebug() << "G = " + QString::number(G);

    ui->lb_progress->setText("Генерация УП...");
    pOutForm->lwOutClear();

    for (int i = 0; i < mHead; ++i) {
        mListFileOut.push_back(mListFile[i]);
    }

    for (auto it = mTypesOfProcessing.end(); it != mTypesOfProcessing.begin();) {
        --it;
        if (it.key() < frame) {
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
        mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G1 " + "X" + QString::number(X) + " Y" + QString::number(Y) + " F" + QString::number(F) + ' ');
    }
    else {
        mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G0 " + "X" + QString::number(X) + " Y" + QString::number(Y) + ' ');
    }
    mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " Z" + QString::number(Z) + ' ');

    for (int i = frame; i < mListFile.size(); ++i) {
        QString str = mListFile[i];
        if (i == mListFile.size() - 1) {
            mListFileOut.push_back(str);
        }
        else {
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
    //ui->lw_file->setCurrentRow(0, QItemSelectionModel::SelectionFlag::SelectCurrent);
    ui->lw_file->setCurrentItem(ui->lw_file->item(ui->spB_findFrame->value()), QItemSelectionModel::SelectionFlag::Select);
    ui->lw_file->scrollToItem(ui->lw_file->item(ui->spB_findFrame->value()), QAbstractItemView::ScrollHint::PositionAtCenter);
    ui->lw_file->setFocus();
}

void MainWindow::on_mb_help_about_triggered() {
    pAboutWindow->exec();
}

void MainWindow::on_mb_file_exit_triggered() {
    close();
}

void MainWindow::rec_processingReady(bool result) {
    if (result) {
        ++mFlag;
    }

    if (mFlag == 2) {
        ui->lw_file->addItems(mListFile);

        setEnabledWidgets(true);
        setEnabledFileWidgets(true);
        ui->lb_tool->setText('T' + mTool);
        ui->lb_countOfFrames->setText(QString::number(mCountOfFrames));
        if (mCountOfFrames - 2 >= 28) {
            ui->spB_stopFrame->setRange(20, mCountOfFrames - 2);
            ui->spB_stopFrame->setValue(20);
            ui->spB_findFrame->setRange(1, mCountOfFrames);
            ui->spB_findFrame->setValue(1);
            ui->statusbar->clearMessage();
        }
        else {
            ui->spB_stopFrame->setEnabled(false);
            emit sig_warning("В программе слишком мало кадров\n"
                             "для расчета возобновления программы.");
        }

        ui->lb_progress->setText("Готово!");
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
    else {
        return;
    }
}

void MainWindow::rec_showMessageError(Errors::Error er) {
    clearAll();
    setEnabledFileWidgets(true);
    pSelectedFile->clear();
    QMessageBox mBox;
    mBox.setWindowTitle(tr("%1").arg(pDictionary->translateString(Errors::error)));
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
    mBox.setWindowTitle(tr("%1").arg(pDictionary->translateString(Errors::error)));
    mBox.setIcon(QMessageBox::Icon::Critical);
    mBox.setText(tr("%1").arg(pDictionary->translateString(Errors::errorDescription[er])));
    mBox.addButton(QMessageBox::Button::Ok);
    mBox.exec();
}

void MainWindow::rec_warning(QString caution) {
    QMessageBox mBox;
    mBox.setWindowTitle("Предупреждение!");
    mBox.setIcon(QMessageBox::Icon::Warning);
    mBox.setText(caution);
    mBox.addButton(QMessageBox::Button::Ok);
    mBox.exec();
}

void MainWindow::rec_showMessageEmptyFile() {
    clearAll();
    QMessageBox mBox;
    mBox.setWindowTitle("Предупреждение!");
    mBox.setIcon(QMessageBox::Icon::Warning);
    mBox.setText("Чтение файла невозможно!\n"
                 "Пустой файл!");
    mBox.addButton(QMessageBox::Button::Ok);
    mBox.exec();
}

void MainWindow::setEnabledFileWidgets(bool enabled) {
    ui->le_pathFile->setEnabled(enabled);
    ui->pb_loadFile->setEnabled(enabled);
    ui->pb_findFile->setEnabled(enabled);
    ui->menubar->setEnabled(enabled);
}

bool MainWindow::setTypeOfProcessing() {
    bool findHead = false;
    for (int i = 0; i < mListFile.size(); ++i) {
        QString strOfProcessing = getStrOfProcessing(i);
        if (strOfProcessing.isEmpty()) {
            continue;
        }
        QString typeOfProcessing = getTypeOfProcessing(strOfProcessing);
        QString frameOfProcessing = getFrameOfProcessing(strOfProcessing);
        ui->lw_typeOfProcessing->addItem(frameOfProcessing + "\t" + pDictionary->translateTypeOfProcessing(typeOfProcessing));
        mTypesOfProcessing[i] = typeOfProcessing;

        if (ui->lw_typeOfProcessing->count() > 0 && !findHead) {
            mHead = i - 1;
            findHead = true;
        }
    }

    if (mTypesOfProcessing.isEmpty()) {
        emit sig_error(Errors::erNoTypeOfProcessing);
        return false;
    }

    if (mHead < 12 || mHead > 12) {
        emit sig_error(Errors::erIncorrectHead);
        return false;
    }

    emit sig_incrementProgressBar();
    return true;
}

QString MainWindow::getStrOfProcessing(int i) {
    if (i < 1 || mListFile.size() - 1 - i < 1) {
        return "";
    }

    QString processingBegin = deleteFrameNumber(mListFile[i - 1]);
    QString processing = deleteFrameNumber(mListFile[i]);
    QString processingEnd = deleteFrameNumber(mListFile[i + 1]);

    if ((processing.size() > 5) &&
        (processing.size() == processingBegin.size() && processing.size() == processingEnd.size()) &&
        (processingBegin.mid(0, 5) == processingEnd.mid(0, 5) && processingBegin.mid(0, 5) == ";(---") &&
        (processing.mid(0, 2) == ";(")) {
        return mListFile[i];
    }
    return "";
}

QString MainWindow::getTypeOfProcessing(QString str) {
    std::string typeOfProcessing = deleteFrameNumber(str).toStdString();
    size_t n = typeOfProcessing.find(" - ");
    if (n != std::string::npos) {
        auto itBegin = typeOfProcessing.begin();
        std::advance(itBegin, n + 3);
        typeOfProcessing.erase(typeOfProcessing.begin(), itBegin);
        auto itEnd = typeOfProcessing.begin();
        while (*itEnd != ')') {
            ++itEnd;
        }
        typeOfProcessing.erase(itEnd, typeOfProcessing.end());
        return QString::fromStdString(typeOfProcessing);
    }
    return "Unknown";
}

QString MainWindow::getFrameOfProcessing(QString str) {
    std::string frameOfProcessing = str.toStdString();
    auto it = std::find(frameOfProcessing.begin(), frameOfProcessing.end(), ' ');
    if (it != frameOfProcessing.end()) {
        frameOfProcessing.erase(it, frameOfProcessing.end());
        return QString::fromStdString(frameOfProcessing);
    }
    return "---";
}

bool MainWindow::setTool() {
    bool toolFinded = false;
    QString settingTool;

    for (int i = 0; i < mListFile.size(); ++i) {
        QString tool = getTool(i);
        if (tool.isEmpty()) {
            continue;
        }
        else {
            if (!toolFinded) {
                toolFinded = true;
                settingTool = tool;
                mTool = tool;
            }
            else if (settingTool == tool) {
                emit sig_warning("Возможно в программе используется разное\n"
                                 "количество оборотов или направление вращения\n"
                                 "для одного инструмента.");
            }
            else {
                emit sig_error(Errors::erOneMoreTool);
                return false;
            }
        }
    }

    if (!toolFinded) {
        emit sig_error(Errors::erNoTool);
        return false;
    }

    emit sig_incrementProgressBar();
    return true;
}

QString MainWindow::getTool(int i) {
    QString str = mListFile[i];

    QString strFindTool = "TC_CHANGETOOL(";
    qsizetype n = str.indexOf(strFindTool);
    if (n != -1) {
        str = str.mid(n + strFindTool.size());
        n = str.indexOf(',');
        if (n != -1) {
            str = str.mid(0, n);
            return str;
        }
    }
    return "";
}

double MainWindow::findValue(int startFrame, QChar command, bool checkIJK) {
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
        }
        else {
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

                return str.toDouble();
            }
        }
    }

    return -999999;
}

void MainWindow::clearAll() {
    mListFile.clear();
    mListFileOut.clear();
    mTool = "-";
    mHead = 0;
    mTypesOfProcessing.clear();
    mCountOfFrames = 0;
    mFlag = 0;

    ui->lw_file->clear();
    ui->lw_typeOfProcessing->clear();
    ui->lb_tool->setText(mTool);
    ui->lb_countOfFrames->setText("-");
    ui->lb_progress->setText("-");
    ui->spB_stopFrame->setValue(0);
    ui->spB_stopFrame->setRange(0, 0);
    ui->spB_stopFrame->setValue(0);
    ui->spB_findFrame->setRange(0, 0);
    ui->spB_findFrame->setValue(0);
    ui->progressBar->setRange(0, ProgressLoadingFile::progressOperationName.size());
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
