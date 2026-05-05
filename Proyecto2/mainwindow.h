#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "lexicalanalyzer.h"
#include "syntaxanalyzer.h"
#include "reportgenerator.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void loadFile();
    void analyze();
    void openReports();

private:
    QTextEdit* codeEditor;
    QTableWidget* tokenTable;
    QTableWidget* errorTable;
    QPushButton* btnLoad;
    QPushButton* btnAnalyze;
    QPushButton* btnReports;
    QLabel* statusLabel;

    std::vector<Token> tokens;
    ErrorManager errorMgr;
    Tablero tablero;
    std::shared_ptr<ASTNode> astRoot;

    void setupUI();
    void populateTokenTable();
    void populateErrorTable();
};

#endif