QT       += core gui svgwidgets multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    board.cpp \
    chessclock.cpp \
    main.cpp \
    mainwindow.cpp \
    chesspiece.cpp \
    promotiondialog.cpp \
    settingswindow.cpp \
    sound.cpp \
    stockfishengine.cpp

HEADERS += \
    board.h \
    chessclock.h \
    mainwindow.h \
    chesspiece.h \
    promotiondialog.h \
    settingswindow.h \
    sound.h \
    stockfishengine.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
