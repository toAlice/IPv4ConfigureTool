QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    signaladapters.cpp

HEADERS += \
    mainwindow.h \
    signaladapters.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    IPv4SettingsSwitch_en_US.ts.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = icon.ico
