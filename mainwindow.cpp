#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    pOutForm = new OutForm(this);
    pAboutWindow = new About(this);
    QPixmap pixmap(":/picture.png");
    setWindowIcon(pixmap);

    pLabel = new QLabel(this);
    pLabel->setVisible(false);
    pProgressBar = new QProgressBar(this);
    pProgressBar->setAlignment(Qt::AlignCenter);
    pProgressBar->setVisible(false);

    ui->progressBar->setValue(0);
    ui->pb_loadFile->setEnabled(false);
    setEnabledWidgets(false);

    QObject::connect(ui->le_pathFile, &QLineEdit::textChanged, this, [&]{
        QFile file(ui->le_pathFile->text());
        file.exists() ? ui->pb_loadFile->setEnabled(true) : ui->pb_loadFile->setEnabled(false); });

    QObject::connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::clearAll);
    QObject::connect(this, &MainWindow::sig_error, this, &MainWindow::rec_showMessageError);
    QObject::connect(this, &MainWindow::sig_errorCalculate, this, &MainWindow::rec_showMessageErrorCalculate);
    QObject::connect(this, &MainWindow::sig_warning, this, &MainWindow::rec_warning);
    QObject::connect(this, &MainWindow::sig_emptyFile, this, &MainWindow::rec_showMessageEmptyFile);
    QObject::connect(this, &MainWindow::sig_incrementProgressBar, this, [&]{ ui->progressBar->setValue(ui->progressBar->value() + 1); });
    QObject::connect(this, &MainWindow::sig_readyReadFile, this, &MainWindow::rec_readyReadFile);
    QObject::connect(&ftrWtchTypeOfProcessingReady, &QFutureWatcher<bool>::finished, this, [&](){ rec_processingReady(ftrTypeOfProcessingReady.result()); });
    QObject::connect(&ftrWtchSetToolReady, &QFutureWatcher<bool>::finished, this, [&](){ rec_processingReady(ftrSetToolReady.result()); });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pb_findFile_clicked() {
    ui->le_pathFile->setText(QFileDialog::getOpenFileName(this, tr("Открыть"), gDir, tr("MPF файлы (*.MPF)")));
    if (mPathFile != ui->le_pathFile->text()) {
        clearAll();
        setEnabledWidgets(false);
        ui->pb_loadFile->setEnabled(true);
    }
}

void MainWindow::on_pb_loadFile_clicked() {
    clearAll();
    ui->progressBar->setRange(0, 5);
    ui->progressBar->setValue(0);
    setEnabledFileWidgets(false);

    ui->lb_progress->setText("Читаю файл...");
    static_cast<void>(QtConcurrent::run([&](){
        mPathFile = ui->le_pathFile->text();
        QFile fileIn(mPathFile);
        if (!fileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit sig_error("Ошибка открытия файла!");
            return;
        }

        while (!fileIn.atEnd()) {
            if (gUTF8) {    // Если текст в локальной 8-бит кодировке (windows-1251);
                mListFile.append(QString::fromUtf8(fileIn.readLine()));
            }
            else {          // Если текст в UTF-8
                mListFile.append(QString::fromLocal8Bit(fileIn.readLine()));
            }
        }
        fileIn.close();

        if (mListFile.size() > 0) {
            emit sig_readyReadFile();
        }
        else {
            setEnabledFileWidgets(true);
            emit sig_emptyFile();
        }
    }));
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
        emit sig_errorCalculate("Ошибка поиска!\n"
                                "'X' не найден.");
        return;
    }
    emit sig_incrementProgressBar();

    Y = findValue(frame, 'Y', true);
    if (Y < chekCommandValue) {
        emit sig_errorCalculate("Ошибка поиска!\n"
                                "'Y' не найден.");
        return;
    }
    emit sig_incrementProgressBar();

    Z = findValue(frame, 'Z', false);
    if (Z < chekCommandValue) {
        emit sig_errorCalculate("Ошибка поиска!\n"
                                "'Z' не найден.");
        return;
    }
    emit sig_incrementProgressBar();

    F = findValue(frame, 'F', false);
    if (F < chekCommandValue) {
        emit sig_errorCalculate("Ошибка поиска!\n"
                                "'X' не найден.");
        return;
    }
    emit sig_incrementProgressBar();

    G = findValue(frame, 'G', false);
    if (G < chekCommandValue) {
        emit sig_errorCalculate("Ошибка поиска!\n"
                                "'X' не найден.");
        return;
    }
    emit sig_incrementProgressBar();

    qDebug() << "Head = " + QString::number(mHead);
    qDebug() << "X = " + QString::number(X);
    qDebug() << "Y = " + QString::number(Y);
    qDebug() << "Z = " + QString::number(Z);
    qDebug() << "F = " + QString::number(F);
    qDebug() << "G = " + QString::number(G);

    pLabel->setText("Генерация УП...");
    pOutForm->lwOutClear();

    for (int i = 0; i < mHead; ++i) {
        mListFileOut.push_back(mListFile[i]);
    }

    //for (auto it = std::prev(mTypesOfProcessing.cend()); it != std::prev(mTypesOfProcessing.cbegin()); --it) {
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
        mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G" + QString::number(G) + " X" + QString::number(X) + " Y" + QString::number(Y) + " F" + QString::number(F) + ' ');
    }
    else {
        mListFileOut.push_back('N' + QString::number(mListFileOut.size()) + " G" + QString::number(G) + " X" + QString::number(X) + " Y" + QString::number(Y) + ' ');
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

void MainWindow::on_mb_help_about_triggered() {
    pAboutWindow->exec();
}

void MainWindow::on_mb_file_exit_triggered() {
    close();
}


void MainWindow::rec_readyReadFile() {
    ui->lb_progress->setText("Обрабатываю файл...");
    emit sig_incrementProgressBar();

    for (int i = 0; i < mListFile.size(); ++i) {
        if (!mListFile[i].isEmpty()) {
            if (mListFile[i][mListFile[i].size() - 1] == '\n') {
                mListFile[i] = mListFile[i].mid(0, mListFile[i].size() - 1);
            }
        }

        if (!mListFile[i].isEmpty()) {
            while (mListFile[i][0] == static_cast<char>(32) || mListFile[i][0] == static_cast<char>(9)) {
                mListFile[i] = mListFile[i].mid(1);
                if (mListFile[i].isEmpty()) {
                    break;
                }
            }
        }

        if (!mListFile[i].isEmpty() && mListFile[i][0] == 'N') {
            ++mCountOfFrames;

            if (i > 0) {
                std::string str = mListFile[i].toStdString();
                size_t n = str.find('N' + std::to_string(i) + ' ');
                if (n == std::string::npos) {
                    emit sig_error("Файл программы поврежден!\n"
                                   "Некорректная нумерация строк.");
                    return;
                }
            }
        }
    }

    for (int i = mListFile.size() - 1; i > 11; --i) {
        if (mListFile[i].isEmpty()) {
            mListFile.pop_back();
        }
        else {
            if (mListFile[i] == "M30" || mListFile[i] == "M30 ") {
                break;
            }
            else {
                emit sig_error("Файл программы поврежден!\n"
                               "Неожиданный конец программы.");
                return;
            }
        }
    }

    emit sig_incrementProgressBar();

    ftrTypeOfProcessingReady = QtConcurrent::run([&]() -> bool { return setTypeOfProcessing(); });
    ftrWtchTypeOfProcessingReady.setFuture(ftrTypeOfProcessingReady);
    ftrSetToolReady = QtConcurrent::run([&]() -> bool { return setTool(); });
    ftrWtchSetToolReady.setFuture(ftrSetToolReady);
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

void MainWindow::rec_showMessageError(QString error) {
    clearAll();
    setEnabledFileWidgets(true);
    mPathFile.clear();
    QMessageBox mBox;
    mBox.setWindowTitle("Ошибка!");
    mBox.setIcon(QMessageBox::Icon::Critical);
    mBox.setText(error);
    mBox.addButton(QMessageBox::Button::Ok);
    mBox.exec();
}

void MainWindow::rec_showMessageErrorCalculate(QString error) {
    pOutForm->close();
    pOutForm->lwOutClear();
    mListFileOut.clear();
    QMessageBox mBox;
    mBox.setWindowTitle("Ошибка!");
    mBox.setIcon(QMessageBox::Icon::Critical);
    mBox.setText(error);
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
        auto it = dictionary.find(typeOfProcessing);
        if (it != dictionary.end()) {
            ui->lw_typeOfProcessing->addItem(frameOfProcessing + "\t" + *it);
        }
        else {
            ui->lw_typeOfProcessing->addItem(frameOfProcessing + "\t" + typeOfProcessing);
        }
        mTypesOfProcessing[i] = typeOfProcessing;

        if (ui->lw_typeOfProcessing->count() > 0 && !findHead) {
            mHead = i - 1;
            findHead = true;
        }
    }

    if (mTypesOfProcessing.isEmpty()) {
        emit sig_error("Программа повреждена!\n"
                       "Не найдено ни одного вида обработки.");
        return false;
    }

    if (mHead < 12 || mHead > 12) {
        emit sig_error("Файл программы поврежден!\n"
                       "Некорректная шапка программы.");
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
                emit sig_error("Обаботка программы невозможна!\n"
                               "В программе используется более одного инструмента.");
                return false;
            }
        }
    }

    if (!toolFinded) {
        emit sig_error("Программа повреждена!\n"
                       "Не найдено ни одного инструмента.");
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
    ui->progressBar->setValue(0);
    ui->spB_stopFrame->setRange(0, 0);
    ui->spB_stopFrame->setValue(0);
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
    ui->pb_calculate->setEnabled(enabled);
    ui->pb_clear->setEnabled(enabled);
    ui->lw_file->setEnabled(enabled);
}
