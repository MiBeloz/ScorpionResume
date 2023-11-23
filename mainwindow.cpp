#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    pOutForm = new OutForm(this);
    pAboutWindow = new About(this);

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
    int frame = ui->spB_stopFrame->value();
    int posFrame = 1;
    double X = 0, Y = 0, Z = 50;
    double F = 0;
    int G = 0;
    if (frame < 1) {
        return;
    }

    qDebug() << QString::fromStdString(std::to_string(frame));

    pProgressBar->setMinimum(0);
    pProgressBar->setMaximum(6);
    pProgressBar->setValue(0);
    pLabel->setMinimumSize(100, 13);
    ui->statusbar->addWidget(pLabel);
    ui->statusbar->addWidget(pProgressBar);
    pLabel->show();
    pProgressBar->show();

    posFrame = findPosFrame(frame);

    pLabel->setText("Поиск 'X'...");
    X = findAxisValue(posFrame, 'X', true);
    pProgressBar->setValue(1);

    pLabel->setText("Поиск 'Y'...");
    Y = findAxisValue(posFrame, 'Y', true);
    pProgressBar->setValue(2);

    pLabel->setText("Поиск 'Z'...");
    Z = findAxisValue(posFrame, 'Z', false);
    pProgressBar->setValue(3);

    pLabel->setText("Поиск 'F'...");
    F = findAxisValue(posFrame, 'F', true);
    pProgressBar->setValue(4);

    pLabel->setText("Поиск 'G'...");
    G = findAxisValue(posFrame, 'G', true);
    pProgressBar->setValue(5);

    qDebug() << "Head = " + QString::number(mHead);

    pLabel->setText("Генерация УП...");
    pOutForm->lwOutClear();
    for (int i = 0; i < mHead; ++i) {
        pOutForm->lwOutAddItem(mListFile[i]);
    }
    for (auto it = std::prev(mTypesOfProcessing.cend()); it != std::prev(mTypesOfProcessing.cbegin()); --it) {
        if (it.key() < posFrame) {
            QString str = ";(";
            for (int j = 0; j < it.value().size(); ++j) {
                str += "-";
            }
            str += ")";
            pOutForm->lwOutAddItem('N' + QString::number(pOutForm->getLwOutCount()) + ' ' + str + ' ');
            pOutForm->lwOutAddItem('N' + QString::number(pOutForm->getLwOutCount()) + " ;(" + it.value() + ")");
            pOutForm->lwOutAddItem('N' + QString::number(pOutForm->getLwOutCount()) + ' ' + str + ' ');

            break;
        }
    }
    pOutForm->lwOutAddItem('N' + QString::number(pOutForm->getLwOutCount()) + " G" + QString::number(G) + " X" + QString::number(X) + " Y" + QString::number(Y) + " F" + QString::number(F) + ' ');
    pOutForm->lwOutAddItem('N' + QString::number(pOutForm->getLwOutCount()) + " Z" + QString::number(Z) + ' ');
    for (int i = posFrame; i < mListFile.size(); ++i) {
        pOutForm->lwOutAddItem(mListFile[i]);
    }

    pProgressBar->setValue(pProgressBar->maximum());
    ui->statusbar->removeWidget(pLabel);
    ui->statusbar->removeWidget(pProgressBar);
    pLabel->close();
    pProgressBar->close();

    pOutForm->show();
}

void MainWindow::on_about_triggered() {
    pAboutWindow->exec();
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

int MainWindow::findPosFrame(int frame) {
    for (int i = 0; i < mListFile[i].size(); ++i) {
        std::string str = mListFile[i].toStdString();
        size_t n = str.find('N' + std::to_string(frame));

        if (n != std::string::npos) {
            return i;
        }
    }
    return 1;
}

double MainWindow::findAxisValue(int posFrame, QChar axis, bool checkIJK) {
    for (int i = posFrame; i > 0; --i) {
        std::string str = mListFile[i].toStdString();

        bool strOK = false;
        if (checkIJK) {
            auto itK = std::find(str.begin(), str.end(), 'K');
            if (itK == str.end()) {
                auto itJ = std::find(str.begin(), str.end(), 'J');
                if (itJ == str.end()) {
                    auto itI = std::find(str.begin(), str.end(), 'I');
                    if (itI == str.end()) {
                        strOK = true;
                    }
                }
            }
        }
        else {
            strOK = true;
        }

        if (strOK) {
            auto it = std::find(str.begin(), str.end(), axis);
            if (it != str.end()) {
                str.erase(str.begin(), ++it);

                auto itEnd = str.begin();
                if (*itEnd == '-') {
                    ++itEnd;
                }
                while (itEnd != str.end() && (std::isdigit(*itEnd) || *itEnd == '.')) {
                    ++itEnd;
                }
                str.erase(itEnd, str.end());

                qDebug() << static_cast<QString>(axis) + " = " + QString::number(std::stod(str));
                return std::stod(str);
            }
        }
    }
    return -100000;
}

void MainWindow::clearAll() {
    mListFile.clear();
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
