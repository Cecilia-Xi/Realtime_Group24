QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
LIBS += -lwiringPi -pthread
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp\
    ../Executive.cpp\
    ../sensor_model/finger_print.cpp\
    ../sensor_model/lib/lib.cpp\
    ../configuration/utils/utils.cpp

HEADERS += \
    mainwindow.h \
    ../cppThread/CppThread.h \
    ../Executive.h\
    ../sensor_model/finger_print.h\
    ../sensor_model/lib/lib.h\
    ../configuration/utils/utils.h\
    ../cppThread/cppThread.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
