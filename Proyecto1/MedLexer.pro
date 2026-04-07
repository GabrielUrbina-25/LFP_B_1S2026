QT       += core gui widgets

CONFIG   += c++17
CONFIG   += console

TARGET   = MedLexer
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/Token.cpp \
    src/LexicalAnalyzer.cpp \
    src/ErrorManager.cpp \
    src/DataStructures.cpp \
    src/ReportGenerator.cpp \
    src/MainWindow.cpp

HEADERS += \
    src/Token.h \
    src/LexicalAnalyzer.h \
    src/ErrorManager.h \
    src/DataStructures.h \
    src/ReportGenerator.h \
    src/MainWindow.h

DESTDIR = $$PWD/bin

# Crear carpeta de reportes en Windows
windows {
    QMAKE_POST_LINK += if not exist $$shell_path($$DESTDIR/reportes) mkdir $$shell_path($$DESTDIR/reportes)
}