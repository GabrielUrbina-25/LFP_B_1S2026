#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void MainWindow::setupUI() {
    setWindowTitle("TaskScript Analyzer - LFP Proyecto 2");
    resize(1200, 800);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);

    // Botones
    auto* btnLayout = new QHBoxLayout();
    btnLoad = new QPushButton("📂 Cargar Archivo .task", this);
    btnAnalyze = new QPushButton("🔍 Analizar", this);
    btnReports = new QPushButton("📊 Ver Reportes", this);
    btnReports->setEnabled(false);

    btnLayout->addWidget(btnLoad);
    btnLayout->addWidget(btnAnalyze);
    btnLayout->addWidget(btnReports);
    btnLayout->addStretch();

    mainLayout->addLayout(btnLayout);

    // Editor
    codeEditor = new QTextEdit(this);
    codeEditor->setPlaceholderText("Cargue un archivo .task o escriba el código aquí...");
    QFont font("Consolas", 11);
    codeEditor->setFont(font);
    mainLayout->addWidget(codeEditor);

    // Tablas
    auto* tablesLayout = new QHBoxLayout();

    tokenTable = new QTableWidget(this);
    tokenTable->setColumnCount(5);
    tokenTable->setHorizontalHeaderLabels(QStringList() << "#" << "Lexema" << "Tipo" << "Línea" << "Columna");

    errorTable = new QTableWidget(this);
    errorTable->setColumnCount(7);
    errorTable->setHorizontalHeaderLabels(QStringList() << "#" << "Lexema" << "Tipo" << "Descripción" << "Línea" << "Columna" << "Severidad");

    tablesLayout->addWidget(tokenTable);
    tablesLayout->addWidget(errorTable);
    mainLayout->addLayout(tablesLayout);

    // Status
    statusLabel = new QLabel("Listo. Cargue un archivo para comenzar.", this);
    mainLayout->addWidget(statusLabel);

    // Conexiones
    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(btnAnalyze, &QPushButton::clicked, this, &MainWindow::analyze);
    connect(btnReports, &QPushButton::clicked, this, &MainWindow::openReports);
}

void MainWindow::loadFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Abrir archivo .task", "", "Task files (*.task)");
    if (filename.isEmpty()) return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    codeEditor->setPlainText(in.readAll());
    file.close();
    statusLabel->setText("Archivo cargado: " + filename);
}

void MainWindow::analyze() {
    errorMgr.clear();
    tokens.clear();
    tokenTable->setRowCount(0);
    errorTable->setRowCount(0);

    std::string input = codeEditor->toPlainText().toStdString();
    if (input.empty()) {
        QMessageBox::warning(this, "Advertencia", "El archivo está vacío.");
        return;
    }

    // Análisis Léxico
    LexicalAnalyzer lexer(input, errorMgr);
    tokens = lexer.getAllTokens();
    populateTokenTable();

    // Análisis Sintáctico
    SyntaxAnalyzer parser(tokens, errorMgr);
    bool success = parser.parse();
    astRoot = parser.getRoot();
    tablero = parser.getTablero();

    populateErrorTable();

    // Crear directorio de reportes
    QString outDir = QDir::currentPath() + "/reportes/";
    QDir().mkpath(outDir);

    ReportGenerator gen(tablero, tokens, errorMgr, astRoot);

    // GENERAR SIEMPRE LOS 3 REPORTES PRINCIPALES + DOT + TOKENS/ERRORES
    gen.generateKanbanReport((outDir + "reporte1_kanban.html").toStdString());
    gen.generateResponsableReport((outDir + "reporte2_responsables.html").toStdString());
    gen.generateColumnasReport((outDir + "reporte3_columnas.html").toStdString());
    gen.generateErrorsReport((outDir + "tokens_errores.html").toStdString());
    gen.generateDOT((outDir + "arbol.dot").toStdString());

    if (!errorMgr.hasErrors() && success) {
        statusLabel->setText("✅ Análisis completado sin errores.");
        btnReports->setEnabled(true);

        QMessageBox::information(this, "Éxito",
                                 "Análisis completado exitosamente.\n\n"
                                 "Reportes generados en:\n" + outDir + "\n\n"
                                                "• reporte1_kanban.html - Tablero visual\n"
                                                "• reporte2_responsables.html - Carga por responsable\n"
                                                "• reporte3_columnas.html - Resumen por columna\n"
                                                "• tokens_errores.html - Tokens y errores\n"
                                                "• arbol.dot - Árbol de derivación (Graphviz)");
    } else {
        statusLabel->setText("⚠️ Análisis completado con errores. Reportes generados con datos parciales.");
        btnReports->setEnabled(true);  // ← AHORA SÍ se habilita

        QMessageBox::warning(this, "Errores encontrados",
                             "El análisis contiene errores, pero los reportes se generaron con los datos válidos extraídos.\n\n"
                             "Revisa la tabla de errores y el reporte tokens_errores.html para más detalles.\n\n"
                             "Reportes generados en:\n" + outDir);
    }
}

void MainWindow::populateTokenTable() {
    tokenTable->setRowCount(tokens.size());
    int i = 0;
    for (const auto& tok : tokens) {
        tokenTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tokenTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tok.lexeme)));
        tokenTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(Token::typeToString(tok.type))));
        tokenTable->setItem(i, 3, new QTableWidgetItem(QString::number(tok.line)));
        tokenTable->setItem(i, 4, new QTableWidgetItem(QString::number(tok.column)));
        i++;
    }
}

void MainWindow::populateErrorTable() {
    auto errors = errorMgr.getErrors();
    errorTable->setRowCount(errors.size());
    int i = 0;
    for (const auto& err : errors) {
        errorTable->setItem(i, 0, new QTableWidgetItem(QString::number(err.number)));
        errorTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(err.lexeme)));
        errorTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(err.type)));
        errorTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(err.description)));
        errorTable->setItem(i, 4, new QTableWidgetItem(QString::number(err.line)));
        errorTable->setItem(i, 5, new QTableWidgetItem(QString::number(err.column)));
        errorTable->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(err.severity)));
        i++;
    }
}

void MainWindow::openReports() {
    QString outDir = QDir::currentPath() + "/reportes/";
    QDesktopServices::openUrl(QUrl::fromLocalFile(outDir + "kanban.html"));
}