#include "mysave.h"
#include <QDebug>

mySave::mySave(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Thread open " << QThread::currentThread();
}

mySave::~mySave(){
    file.flush();
    file.close();
    qDebug() << "File close";
}

void mySave::setFile(QString fileName){
    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly |QIODevice::Append| QIODevice::Text))
        return;
}

void mySave::writeFile(QString writeValue)
{
    QTextStream out(&file);
    out << writeValue;
}

void mySave::closeFile(){
    file.flush();
    file.close();
}
