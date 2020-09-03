#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T13:24:01
#
#-------------------------------------------------

QT       += core gui sql
QT       += network
#win32:debug{
#CONFIG += console
#}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ocr-client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    libhocr/tinyxml2/tinyxml2.cpp \
    libhocr/hocr_node.cpp \
    libhocr/hocr_parser.cpp \
    anotherlogin.cpp \
    clusterproof.cpp \
    clustertextitem.cpp \
    editboard.cpp \
    fullpicproof.cpp \
    itemrecognizer.cpp \
    login.cpp \
    main.cpp \
    mainwindow.cpp \
    mydatabase.cpp \
    myitem.cpp \
    mylistwidget.cpp \
    mypixmapitem.cpp \
    myscene.cpp \
    mysingletextdialog.cpp \
    mytextitem.cpp \
    mytextscene.cpp \
    myview.cpp \
    networkclub.cpp \
    proofboard.cpp \
    procedureselect.cpp


HEADERS += \
    libhocr/tinyxml2/tinyxml2.h \
    libhocr/hocr_node.h \
    libhocr/hocr_parser.h \
    libhocr/hocr_types.h \
    anotherlogin.h \
    clusterproof.h \
    clustertextitem.h \
    config.h \
    editboard.h \
    fullpicproof.h \
    itemrecognizer.h \
    login.h \
    mainwindow.h \
    message.h \
    mydatabase.h \
    myitem.h \
    mylistwidget.h \
    mypixmapitem.h \
    myscene.h \
    mysingletextdialog.h \
    mytextitem.h \
    mytextscene.h \
    myview.h \
    networkclub.h \
    proofboard.h \
    procedureselect.h


FORMS += \
    mainwindow.ui \
    login.ui \
    editboard.ui \
    proofboard.ui \
    fullpicproof.ui \
    clusterproof.ui \
    mysingletextdialog.ui \
    anotherlogin.ui \
    procedureselect.ui


RESOURCES += \
    resouce.qrc \


