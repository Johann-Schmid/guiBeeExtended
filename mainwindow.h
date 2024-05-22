#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineSeries>
#include <QChart>
#include <QChartView>
#include "mythread.h"
#include "qcustomplot.h"
#include <stdint.h>
#include "mysave.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef struct lastData {
    uint32_t time;
    uint boValue;
}lastData;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    lastData arrayLastData[6] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}};
    QThread *thread = nullptr;
    myThread *mThread = nullptr;
    QThread *threadSave = nullptr;
    mySave *mSave = nullptr;

    QTimer dataTimer;

    QString fileName = nullptr;
    QComboBox* comboBox;
    QList<QString> interfaceList;
    QString interfaceValue;

    void setLastData(uint, uint, uint);
    void setupRealtimeDataDemo(QCustomPlot *customPlot);

private slots:
    void setEditText(const QByteArray &);
    void realtimeDataSlot();
    void setInterface(int);

    void on_actionFile_triggered();
    void on_actionStart_triggered();
    void on_actionStop_triggered();
    void on_actionFind_Interface_triggered();
    void on_actionInfo_triggered();
    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
