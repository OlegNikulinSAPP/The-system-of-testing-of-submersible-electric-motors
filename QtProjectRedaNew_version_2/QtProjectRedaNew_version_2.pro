QT       += core gui sql serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += qaxcontainer
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
        RC_FILE += logo.rc
        OTHER_FILES += logo.rc
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    password.cpp \
    window_experience_1.cpp \
    window_experience_10.cpp \
    window_experience_11.cpp \
    window_experience_12.cpp \
    window_experience_13.cpp \
    window_experience_14.cpp \
    window_experience_15.cpp \
    window_experience_2.cpp \
    window_experience_3.cpp \
    window_experience_4.cpp \
    window_experience_5.cpp \
    window_experience_6.cpp \
    window_experience_7.cpp \
    window_experience_8.cpp \
    window_experience_9.cpp

HEADERS += \
    mainwindow.h \
    password.h \
    window_experience_1.h \
    window_experience_10.h \
    window_experience_11.h \
    window_experience_12.h \
    window_experience_13.h \
    window_experience_14.h \
    window_experience_15.h \
    window_experience_2.h \
    window_experience_3.h \
    window_experience_4.h \
    window_experience_5.h \
    window_experience_6.h \
    window_experience_7.h \
    window_experience_8.h \
    window_experience_9.h

FORMS += \
    mainwindow.ui \
    password.ui \
    window_experience_1.ui \
    window_experience_10.ui \
    window_experience_11.ui \
    window_experience_12.ui \
    window_experience_13.ui \
    window_experience_14.ui \
    window_experience_15.ui \
    window_experience_2.ui \
    window_experience_3.ui \
    window_experience_4.ui \
    window_experience_5.ui \
    window_experience_6.ui \
    window_experience_7.ui \
    window_experience_8.ui \
    window_experience_9.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc
