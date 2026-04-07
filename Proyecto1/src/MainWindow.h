#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <vector>

#include "Token.h"
#include "ErrorManager.h"
#include "DataStructures.h"
#include "LexicalAnalyzer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadFile();
    void analyze();
    void showReports();
    void showTokenStats();
    void generarImagenGraphviz();

private:
    void setupUI();
    void populateTokenTable(const std::vector<Token>& tokens);
    void populateErrorTable();
    void parseSemanticData(const std::vector<Token>& tokens);
    void generateAllReports();

    bool isEspecialidad(TokenType t);
    bool isDosis(TokenType t);

    QTextEdit* codeEditor;
    QTableWidget* tokenTable;
    QTableWidget* errorTable;
    QPushButton* btnLoad;
    QPushButton* btnAnalyze;
    QPushButton* btnReports;
    QPushButton* btnStats;
    QLabel* statusLabel;

    ErrorManager* errorManager;
    HospitalData* hospitalData;
    std::vector<Token> lastTokens;
    AnalysisStats lastStats;
    QString currentFilePath;
};

#endif