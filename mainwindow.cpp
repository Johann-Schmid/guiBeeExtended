#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"
#include "mysave.h"
#include <QThread>
#include <QFileDialog>
#include <QStandardPaths>
#include <QChartView>
#include <QSerialPortInfo>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setupRealtimeDataDemo(ui->customPlot);

    comboBox = new QComboBox();
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setInterface(int)));

    thread = new QThread(this);
    mThread = new myThread();
    threadSave = new QThread(this);
    mSave = new mySave();
}

MainWindow::~MainWindow()
{
    if (thread->isRunning()){
        thread->quit();
        thread->wait();
    }
    delete thread;
    delete mThread;

    if (threadSave->isRunning()){
        threadSave->quit();
        threadSave->wait();
    }
    delete threadSave;
    delete mSave;

    delete ui;
}

void MainWindow::setEditText(const QByteArray &text){
    qDebug() << "Main: " << text;
    ui->textEdit->moveCursor (QTextCursor::End);
    ui->textEdit->insertPlainText (text);

    QMetaObject::invokeMethod(mSave, "writeFile", Q_ARG(QString, QString::fromUtf8(text)));

    QString list = QString::fromUtf8(text.constData(), text.length()).simplified();
    QStringList stringList = list.split(';');
    QString selectChannel = stringList[0].right(1); // Assuming the channel number is a single digit at the end

    bool ok;
    uint channel = selectChannel.toUInt(&ok);
    if (ok && channel < 6) {
        setLastData(channel, stringList[1].toUInt(), stringList[2].toUInt());
    }

    qDebug() << stringList;
}

void MainWindow::setLastData(uint channel, uint time, uint boValue){
    if (channel < 6) {
        arrayLastData[channel].time = time;
        arrayLastData[channel].boValue = boValue;
    }
}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    customPlot->addGraph(); // red line
    customPlot->graph(0)->setPen(QPen(Qt::darkRed));
    customPlot->addGraph(); // green line
    customPlot->graph(1)->setPen(QPen(Qt::darkGreen));
    customPlot->addGraph(); // cyan line
    customPlot->graph(2)->setPen(QPen(Qt::blue));
    customPlot->addGraph(); // magenta line
    customPlot->graph(3)->setPen(QPen(Qt::green));
    customPlot->addGraph(); // magenta line
    customPlot->graph(4)->setPen(QPen(Qt::magenta));
    customPlot->addGraph(); // magenta line
    customPlot->graph(5)->setPen(QPen(Qt::red));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s:%z");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-0.1, 6.6);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
}

void MainWindow::realtimeDataSlot()
{
    static QTime timeStart = QTime::currentTime();
    double key = timeStart.msecsTo(QTime::currentTime()) / 1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;

    if (key - lastPointKey > 0.002) {
        ui->customPlot->graph(0)->addData(key, arrayLastData[0].boValue);
        ui->customPlot->graph(1)->addData(key, arrayLastData[1].boValue + 1.1);
        ui->customPlot->graph(2)->addData(key, arrayLastData[2].boValue + 2.2);
        ui->customPlot->graph(3)->addData(key, arrayLastData[3].boValue + 3.3);
        ui->customPlot->graph(4)->addData(key, arrayLastData[4].boValue + 4.4);
        ui->customPlot->graph(5)->addData(key, arrayLastData[5].boValue + 5.5);
        lastPointKey = key;
    }

    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key - lastFpsKey > 2) {
        ui->statusBar->showMessage(
            QString("%1 FPS, Total Data points: %2")
                .arg(frameCount / (key - lastFpsKey), 0, 'f', 0)
                .arg(ui->customPlot->graph(0)->data()->size() + ui->customPlot->graph(1)->data()->size()),
            0);
        lastFpsKey = key;
        frameCount = 0;
    }
}

void MainWindow::on_actionFile_triggered()
{
    fileName = QFileDialog::getSaveFileName(this,
                                            tr("Save Values"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QDir::separator() + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh-mm-ss"), tr("Text files (*.txt)"));
    qDebug() << fileName;

    bool enableStart = (fileName != nullptr);
    ui->actionFind_Interface->setEnabled(enableStart);

    if (threadSave->isRunning() == false) {
        ui->actionStart->setEnabled(enableStart);
    }
}

void MainWindow::on_actionStart_triggered()
{
    connect(mThread, SIGNAL(emitData(QByteArray)), this, SLOT(setEditText(QByteArray)));

    if (thread->isRunning() == false) {
        mThread->moveToThread(thread);
        thread->start();
    }

    if (threadSave->isRunning() == false) {
        mSave->moveToThread(threadSave);
        threadSave->start();
    }

    QMetaObject::invokeMethod(mThread, "setInterfaceName", Q_ARG(QString, interfaceValue));
    QMetaObject::invokeMethod(mSave, "setFile", Q_ARG(QString, fileName));

    dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}

void MainWindow::on_actionStop_triggered()
{
    if (thread->isRunning()) {
        QMetaObject::invokeMethod(mThread, "closeInterface");
    }

    if (threadSave->isRunning()) {
        QMetaObject::invokeMethod(mSave, "closeFile");
    }
    disconnect(mThread, SIGNAL(emitData(QByteArray)), this, SLOT(setEditText(QByteArray)));
    ui->actionStart->setEnabled(false);
    fileName = nullptr;
    dataTimer.stop();
}

void MainWindow::on_actionFind_Interface_triggered()
{
    ui->toolBar->addWidget(comboBox);
    comboBox->clear();

    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        if (info.vendorIdentifier() == 0x04B4 && info.productIdentifier() == 0xF139) {
            qDebug() << "SystemLocation" << info.systemLocation();
            interfaceList.append(info.systemLocation());
            comboBox->addItem(info.portName());
        }
    }
}

void MainWindow::on_actionInfo_triggered()
{
    QMessageBox::information(this, tr("BeeGui 2V0"),
                             tr("Contact me: johann.schmid@ur.de"),
                             QMessageBox::Ok);
}

void MainWindow::setInterface(int currentIndex)
{
    qDebug() << currentIndex;
    qDebug() << interfaceList.value(currentIndex);
    if (currentIndex > -1 && !fileName.isEmpty()) {
        interfaceValue = interfaceList.value(currentIndex);
        ui->actionStart->setEnabled(true);
    }
    else {
        ui->actionStart->setEnabled(false);
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}
