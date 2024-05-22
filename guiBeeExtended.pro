QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
VERSION = 1.0.1.0

DEFINES +=

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mysave.cpp \
    mythread.cpp \
    qcustomplot.cpp

HEADERS += \
    mainwindow.h \
    mysave.h \
    mythread.h \
    qcustomplot.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    icons.qrc

QMAKE_CXXFLAGS += -Wa,-mbig-obj
