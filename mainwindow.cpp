#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBarAll->setMinimum(0);
    ui->progressBarAll->setValue(0);
    ui->progressBarLocal->setMinimum(0);
    ui->progressBarLocal->setValue(0);

    setEnabledWidgwts(false);

    pAboutWindow = new About(this);

    QObject::connect(this, &MainWindow::sig_readyReadFile, this, &MainWindow::rec_readyReadFile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::rec_readyReadFile(QStringList strList)
{
    ui->progressBarLocal->setMinimum(0);
    ui->progressBarLocal->setMaximum(strList.size() + 1);
    ui->progressBarLocal->setValue(0);
    ui->lb_progress->setText("Чтение файла...");

    size_t countOfFrames = 0;

    for (int i = 0; i < strList.size(); ++i) {
        if (strList[i][strList[i].size() - 1] == '\n') {
            strList[i] = strList[i].mid(0, strList[i].size() - 1);
        }

        while (strList[i][0] == 32 || strList[i][0] == 9) {
            strList[i] = strList[i].mid(1);
            if (strList[i].size() <= 0) {
                break;
            }
        }

        if (strList[i].size() > 0 && (strList[i][0] == 'N' || strList[i][0] == 'M')) {
            ++countOfFrames;
        }

        ui->lw_file->addItem(strList[i]);

        ui->progressBarLocal->setValue(i + 1);
    }
    ui->progressBarLocal->setValue(ui->progressBarLocal->maximum());
    ui->progressBarAll->setValue(1);

    setTypeOfProcessing();
    setTool();

    setEnabledWidgwts(true);

    //qDebug() << strList.size();
    //qDebug() << ui->lw_file->count();

    ui->lb_countOfFrames->setText(QString::number(countOfFrames));
    ui->lb_tool->setText(mTool);
    ui->lb_progress->setText("Готово!");

    ui->progressBarAll->setValue(ui->progressBarAll->maximum());
}

void MainWindow::setEnabledWidgwts(bool enabled)
{
    ui->grB_info->setEnabled(enabled);
    ui->grB_settings->setEnabled(enabled);
    ui->pb_ok->setEnabled(enabled);
    ui->pb_clear->setEnabled(enabled);
    ui->lw_file->setEnabled(enabled);
}

void MainWindow::on_about_triggered()
{
    pAboutWindow->exec();
}

void MainWindow::on_pb_findFile_clicked()
{
    ui->le_pathFile->setText(QFileDialog::getOpenFileName(this, tr("Открыть"), mDir, tr("MPF файлы (*.MPF)")));

    clearAll();
    setEnabledWidgwts(false);

    ui->progressBarAll->setMinimum(0);
    ui->progressBarAll->setMaximum(4);
    ui->progressBarAll->setValue(0);

    mFile.setFileName(ui->le_pathFile->text());

    QtConcurrent::run([&](){ if (!mFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                return;
                            }

                            QStringList strList;
                            while (!mFile.atEnd()) {
                                QByteArray data = mFile.readLine();

                                // Если текст в локальной 8-бит кодировке (windows-1251);
                                QString fileLine = QString::fromLocal8Bit(data);

                                // Если текст в UTF-8
                                //text = QString::fromUtf8(data);

                                strList.append(fileLine);
                            }
                            mFile.close();

                            sig_readyReadFile(strList);
    });
}

void MainWindow::setTypeOfProcessing()
{
    ui->progressBarLocal->setMinimum(0);
    ui->progressBarLocal->setMaximum(ui->lw_file->count() + 1);
    ui->progressBarLocal->setValue(0);
    ui->lb_progress->setText("Чтение видов обработок...");

    bool findHead = false;
    for (int i = 0; i < ui->lw_file->count(); ++i) {
        getTypeOfProcessing("HSR-", i);
        getTypeOfProcessing("HSM-", i);
        if (ui->lw_typeOfProcessing->count() > 0 && !findHead) {
            head = i;
            size_t n = 0;
            do {
                head--;
                std::string str = ui->lw_file->item(head)->text().toStdString();
                std::string strFind = 'N' + std::to_string(head) + " ;";
                n = str.find(strFind);
            }
            while (n != std::string::npos);
            findHead = true;
        }

        ui->progressBarLocal->setValue(i + 1);
    }
    ui->progressBarLocal->setValue(ui->progressBarLocal->maximum());
    ui->progressBarAll->setValue(2);
}

void MainWindow::getTypeOfProcessing(QString type, int i)
{
    std::string str = ui->lw_file->item(i)->text().toStdString();
    size_t j = str.find(type.toStdString());

    if (j != std::string::npos) {
        //qDebug() << j;
        {
            auto it = str.begin();
            std::advance(it, j);
            str.erase(str.begin(), it);
        }
        {
            auto it = std::find(str.begin(), str.end(), ')');
            if (it != str.end()) {
                str.erase(it, str.end());
                ui->lw_typeOfProcessing->addItem(QString::fromStdString(str));
            }
        }
    }
}

void MainWindow::setTool()
{
    ui->progressBarLocal->setMinimum(0);
    ui->progressBarLocal->setMaximum(ui->lw_file->count() + 1);
    ui->progressBarLocal->setValue(0);
    ui->lb_progress->setText("Чтение данных инструмента...");

    for (int i = 0; i < ui->lw_file->count(); ++i) {
        getTool("TC_CHANGETOOL(", i);

        ui->progressBarLocal->setValue(i + 1);
    }
    ui->progressBarLocal->setValue(ui->progressBarLocal->maximum());
    ui->lb_progress->setText("Почти готово, подождите...");
    ui->progressBarAll->setValue(3);
}

void MainWindow::getTool(QString type, int i)
{
    std::string str = ui->lw_file->item(i)->text().toStdString();
    size_t j = str.find(type.toStdString());

    if (j != std::string::npos) {
        //qDebug() << j;
        {
            auto it = str.begin();
            std::advance(it, j + 14);
            str.erase(str.begin(), it);
        }
        {
            auto it = std::find(str.begin(), str.end(), ',');
            if (it != str.end()) {
                str.erase(it, str.end());

                if (QString::fromStdString(str).size() > 0) {
                    while (QString::fromStdString(str)[0] == 32 || QString::fromStdString(str)[0] == 9) {
                        QString::fromStdString(str) = QString::fromStdString(str).mid(1);
                    }
                }

                if (ui->lb_tool->text() == "-") {
                    mTool = 'T' + QString::fromStdString(str);
                }
                else {
                    mTool = ui->lb_tool->text() + ", T" + QString::fromStdString(str);
                }
            }
        }
    }
}

void MainWindow::clearAll()
{
    ui->lw_file->clear();
    ui->lw_typeOfProcessing->clear();
    ui->lb_tool->setText("-");
    mTool = "-";
    ui->lb_countOfFrames->setText("-");
    ui->lb_progress->setText("-");
    ui->spB_stopFrame->setValue(0);
    ui->progressBarAll->setValue(0);
    ui->progressBarLocal->setValue(0);
}

void MainWindow::on_pb_ok_clicked()
{
    int frame = ui->spB_stopFrame->value();
    int posFrame = 1;
    double X = 0, Y = 0, Z = 50;
    double F = 0;
    int G = 0;
    if (frame < 1) {
        return;
    }

    qDebug() << QString::fromStdString(std::to_string(frame));

    for (int i = 0; i < ui->lw_file->count(); ++i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        size_t n = str.find('N' + std::to_string(frame));

        if (n != std::string::npos) {
            posFrame = i;
            break;
        }
    }

    for (int i = posFrame - 1; i > 0; --i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        auto it = std::find(str.begin(), str.end(), 'X');
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
            X = std::stod(str);

            qDebug() << "X = " + QString::number(X);

            break;
        }
    }

    for (int i = posFrame - 1; i > 0; --i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        auto it = std::find(str.begin(), str.end(), 'Y');
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
            Y = std::stod(str);

            qDebug() << "Y = " + QString::number(Y);

            break;
        }
    }

    for (int i = posFrame - 1; i > 0; --i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        auto it = std::find(str.begin(), str.end(), 'Z');
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
            Z = std::stod(str);

            qDebug() << "Z = " + QString::number(Z);

            break;
        }
    }

    for (int i = posFrame - 1; i > 0; --i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        auto it = std::find(str.begin(), str.end(), 'F');
        if (it != str.end()) {
            str.erase(str.begin(), ++it);

            auto itEnd = str.begin();
            while (itEnd != str.end() && (std::isdigit(*itEnd) || *itEnd == '.')) {
                ++itEnd;
            }
            str.erase(itEnd, str.end());
            F = std::stod(str);

            qDebug() << "F = " + QString::number(F);

            break;
        }
    }

    for (int i = posFrame - 1; i > 0; --i) {
        std::string str = ui->lw_file->item(i)->text().toStdString();
        auto it = std::find(str.begin(), str.end(), 'G');
        if (it != str.end()) {
            str.erase(str.begin(), ++it);

            auto itEnd = str.begin();
            while (itEnd != str.end() && (std::isdigit(*itEnd) || *itEnd == '.')) {
                ++itEnd;
            }
            str.erase(itEnd, str.end());
            G = std::stoi(str);

            qDebug() << "G = " + QString::number(G);
            qDebug() << "Head = " + QString::number(head);

            break;
        }
    }
}
