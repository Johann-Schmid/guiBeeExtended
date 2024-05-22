#include "mainwindow.h"
#include <QLatin1String>

#include <QApplication>
#include <QtWidgets>
#include <QFile>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    QFile styleSheetFile("Adaptic.qss");
//    styleSheetFile.open(QFile::ReadOnly);
//    QString styleSheet = QLatin1String(styleSheetFile.readAll());
//    a.setStyleSheet(styleSheet);

    qDebug() << a.applicationVersion();
    qDebug() << a.applicationName();

    MainWindow w;

    a.setApplicationName(a.applicationName() + " " + a.applicationVersion());
    w.setWindowTitle( a.applicationName() );

    w.show();
    return a.exec();
}

