QT       += core gui widgets

CONFIG += c++17

TARGET = TaskScriptAnalyzer
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    token.cpp \
    lexicalanalyzer.cpp \
    syntaxanalyzer.cpp \
    errormanager.cpp \
    reportgenerator.cpp \
    kanbandata.cpp

HEADERS += \
    mainwindow.h \
    token.h \
    lexicalanalyzer.h \
    syntaxanalyzer.h \
    errormanager.h \
    reportgenerator.h \
    kanbandata.h