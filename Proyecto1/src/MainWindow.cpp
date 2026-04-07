#include "MainWindow.h"
#include "ReportGenerator.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    errorManager(new ErrorManager()),
    hospitalData(new HospitalData()) {
    setupUI();
    setWindowTitle("MedLexer - Analizador Lexico Hospitalario");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
    delete errorManager;
    delete hospitalData;
}

void MainWindow::setupUI() {
    auto* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto* mainLayout = new QVBoxLayout(centralWidget);

    auto* toolbar = new QHBoxLayout();
    btnLoad = new QPushButton("Cargar Archivo .med");
    btnAnalyze = new QPushButton("Analizar");
    btnReports = new QPushButton("Ver Reportes HTML");
    btnStats = new QPushButton("Estadisticas");
    QPushButton* btnGraphviz = new QPushButton("Generar Imagen Graphviz");

    btnAnalyze->setEnabled(false);
    btnReports->setEnabled(false);
    btnStats->setEnabled(false);

    toolbar->addWidget(btnLoad);
    toolbar->addWidget(btnAnalyze);
    toolbar->addWidget(btnReports);
    toolbar->addWidget(btnStats);
    toolbar->addStretch();
    toolbar->addWidget(btnGraphviz);
    mainLayout->addLayout(toolbar);

    auto* splitter = new QSplitter(Qt::Vertical);

    auto* editorGroup = new QGroupBox("Codigo Fuente (.med)");
    auto* editorLayout = new QVBoxLayout(editorGroup);
    codeEditor = new QTextEdit();
    codeEditor->setFont(QFont("Consolas", 11));
    codeEditor->setPlaceholderText("Cargue un archivo .med para comenzar...");
    editorLayout->addWidget(codeEditor);
    splitter->addWidget(editorGroup);

    auto* tablesWidget = new QWidget();
    auto* tablesLayout = new QHBoxLayout(tablesWidget);

    auto* tokenGroup = new QGroupBox("Tabla de Tokens");
    auto* tokenLayout = new QVBoxLayout(tokenGroup);
    tokenTable = new QTableWidget();
    tokenTable->setColumnCount(5);
    tokenTable->setHorizontalHeaderLabels(QStringList() << "#" << "Lexema" << "Tipo" << "Linea" << "Columna");
    tokenLayout->addWidget(tokenTable);
    tablesLayout->addWidget(tokenGroup);

    auto* errorGroup = new QGroupBox("Tabla de Errores Lexicos");
    auto* errorLayout = new QVBoxLayout(errorGroup);
    errorTable = new QTableWidget();
    errorTable->setColumnCount(6);
    errorTable->setHorizontalHeaderLabels(QStringList() << "No." << "Lexema" << "Tipo" << "Descripcion" << "Linea" << "Columna");
    errorLayout->addWidget(errorTable);
    tablesLayout->addWidget(errorGroup);

    splitter->addWidget(tablesWidget);
    mainLayout->addWidget(splitter);

    statusLabel = new QLabel("Listo. Cargue un archivo para comenzar.");
    mainLayout->addWidget(statusLabel);

    connect(btnLoad, &QPushButton::clicked, this, &MainWindow::loadFile);
    connect(btnAnalyze, &QPushButton::clicked, this, &MainWindow::analyze);
    connect(btnReports, &QPushButton::clicked, this, &MainWindow::showReports);
    connect(btnStats, &QPushButton::clicked, this, &MainWindow::showTokenStats);
    connect(btnGraphviz, &QPushButton::clicked, this, &MainWindow::generarImagenGraphviz);
}

void MainWindow::loadFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Abrir archivo", "", "Archivos MED (*.med)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo");
        return;
    }

    QTextStream in(&file);
    codeEditor->setPlainText(in.readAll());
    currentFilePath = fileName;
    btnAnalyze->setEnabled(true);
    statusLabel->setText("Archivo cargado: " + fileName);
}

void MainWindow::analyze() {
    QString content = codeEditor->toPlainText();
    if (content.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "No hay contenido para analizar");
        return;
    }

    delete hospitalData;
    hospitalData = new HospitalData();
    errorManager->clear();
    lastTokens.clear();

    LexicalAnalyzer analyzer(content.toStdString(), *errorManager);
    analyzer.startStatsCollection();

    Token token = analyzer.nextToken();
    while (token.type != TokenType::END_OF_FILE) {
        lastTokens.push_back(token);
        token = analyzer.nextToken();
    }

    analyzer.endStatsCollection();
    lastStats = analyzer.getStats();

    populateTokenTable(lastTokens);
    populateErrorTable();
    parseSemanticData(lastTokens);
    generateAllReports();

    QString status = QString("Tokens: %1 | Errores: %2 | Tiempo: %3 ms")
                         .arg(lastTokens.size())
                         .arg(errorManager->getErrors().size())
                         .arg(lastStats.analysisTimeMs, 0, 'f', 2);
    statusLabel->setText(status);

    btnReports->setEnabled(true);
    btnStats->setEnabled(true);

    if (errorManager->hasErrors()) {
        QMessageBox::warning(this, "Completado con errores",
                             QString("Se encontraron %1 errores. Revise la tabla.").arg(errorManager->getErrors().size()));
    } else {
        QMessageBox::information(this, "Exito", "Analisis completado. Reportes generados.");
    }
}

void MainWindow::populateTokenTable(const std::vector<Token>& tokens) {
    tokenTable->setRowCount(tokens.size());
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& t = tokens[i];
        tokenTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tokenTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.lexeme)));
        tokenTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(Token::typeToString(t.type))));
        tokenTable->setItem(i, 3, new QTableWidgetItem(QString::number(t.line)));
        tokenTable->setItem(i, 4, new QTableWidgetItem(QString::number(t.column)));
    }
}

void MainWindow::populateErrorTable() {
    const auto& errors = errorManager->getErrors();
    errorTable->setRowCount(errors.size());
    for (size_t i = 0; i < errors.size(); ++i) {
        const auto& e = errors[i];
        errorTable->setItem(i, 0, new QTableWidgetItem(QString::number(e.number)));
        errorTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(e.lexeme)));
        errorTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(e.type)));
        errorTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(e.description)));
        errorTable->setItem(i, 4, new QTableWidgetItem(QString::number(e.line)));
        errorTable->setItem(i, 5, new QTableWidgetItem(QString::number(e.column)));
    }
}

void MainWindow::parseSemanticData(const std::vector<Token>& tokens) {
    enum class Context { NONE, HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS };
    Context ctx = Context::NONE;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& t = tokens[i];

        // Detectar secciones
        if (t.type == TokenType::HOSPITAL) {
            ctx = Context::HOSPITAL;
            if (i + 1 < tokens.size() && tokens[i+1].type == TokenType::STRING) {
                hospitalData->nombre = tokens[i+1].lexeme.substr(1, tokens[i+1].lexeme.length()-2);
            }
        }
        else if (t.type == TokenType::PACIENTES) ctx = Context::PACIENTES;
        else if (t.type == TokenType::MEDICOS) ctx = Context::MEDICOS;
        else if (t.type == TokenType::CITAS) ctx = Context::CITAS;
        else if (t.type == TokenType::DIAGNOSTICOS) ctx = Context::DIAGNOSTICOS;

        // Parsear PACIENTE
        if (t.type == TokenType::PACIENTE && ctx == Context::PACIENTES) {
            if (i + 2 < tokens.size() && tokens[i+2].type == TokenType::STRING) {
                Paciente p;
                p.nombre = tokens[i+2].lexeme.substr(1, tokens[i+2].lexeme.length()-2);

                size_t j = i + 3;
                while (j < tokens.size() && tokens[j].type != TokenType::RBRACKET) {
                    if (tokens[j].type == TokenType::EDAD) {
                        if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::INTEGER) {
                            p.edad = std::stoi(tokens[j+2].lexeme);
                        }
                    }
                    else if (tokens[j].type == TokenType::TIPO_SANGRE) {
                        if (j + 2 < tokens.size() &&
                            (tokens[j+2].type == TokenType::BLOOD_TYPE || tokens[j+2].type == TokenType::STRING)) {
                            std::string sangre = tokens[j+2].lexeme;
                            if (sangre.length() >= 2 && sangre[0] == '"' && sangre.back() == '"') {
                                p.tipoSangre = sangre.substr(1, sangre.length() - 2);
                            } else {
                                p.tipoSangre = sangre;
                            }
                        }
                    }
                    else if (tokens[j].type == TokenType::HABITACION) {
                        if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::INTEGER) {
                            p.habitacion = std::stoi(tokens[j+2].lexeme);
                        }
                    }
                    j++;
                }
                hospitalData->pacientes.push_back(p);
            }
        }
        // Parsear MEDICO
        if (t.type == TokenType::MEDICO && ctx == Context::MEDICOS) {
            if (i + 2 < tokens.size() && tokens[i+2].type == TokenType::STRING) {
                Medico m;
                m.nombre = tokens[i+2].lexeme.substr(1, tokens[i+2].lexeme.length()-2);
                m.citasProgramadas = 0;

                size_t j = i + 3;
                while (j < tokens.size() && tokens[j].type != TokenType::RBRACKET) {
                    if (tokens[j].type == TokenType::ESPECIALIDAD) {
                        if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                            if (j + 2 < tokens.size()) {
                                TokenType esp = tokens[j+2].type;
                                if (esp == TokenType::CARDIOLOGIA || esp == TokenType::NEUROLOGIA ||
                                    esp == TokenType::PEDIATRIA || esp == TokenType::CIRUGIA ||
                                    esp == TokenType::MEDICINA_GENERAL || esp == TokenType::ONCOLOGIA) {
                                    m.especialidad = Token::typeToString(esp);
                                    j += 2;
                                }
                            }
                        }
                    }
                    else if (tokens[j].type == TokenType::CODIGO) {
                        if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                            if (j + 2 < tokens.size()) {
                                if (tokens[j+2].type == TokenType::CODE_ID) {
                                    m.codigo = tokens[j+2].lexeme;
                                    j += 2;
                                }
                                else if (tokens[j+2].type == TokenType::STRING) {
                                    std::string cod = tokens[j+2].lexeme;
                                    if (cod.length() >= 2 && cod[0] == '"' && cod.back() == '"') {
                                        m.codigo = cod.substr(1, cod.length() - 2);
                                    } else {
                                        m.codigo = cod;
                                    }
                                    j += 2;
                                }
                            }
                        }
                    }
                    j++;
                }
                hospitalData->medicos.push_back(m);
            }
        }

        // Parsear CITA
        if (t.type == TokenType::CITA && ctx == Context::CITAS) {
            if (i + 2 < tokens.size() && tokens[i+2].type == TokenType::STRING) {
                Cita c;
                c.paciente = tokens[i+2].lexeme.substr(1, tokens[i+2].lexeme.length()-2);

                size_t j = i + 3;
                while (j < tokens.size() && tokens[j].type != TokenType::CON) j++;

                if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::STRING) {
                    c.medico = tokens[j+1].lexeme.substr(1, tokens[j+1].lexeme.length()-2);
                }

                while (j < tokens.size() && tokens[j].type != TokenType::LBRACKET) j++;
                j++;

                while (j < tokens.size() && tokens[j].type != TokenType::RBRACKET) {
                    if (tokens[j].type == TokenType::FECHA) {
                        if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                            if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::DATE_LITERAL) {
                                c.fecha = tokens[j+2].lexeme;
                            }
                        }
                    }
                    if (tokens[j].type == TokenType::HORA) {
                        if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                            if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::TIME_LITERAL) {
                                c.hora = tokens[j+2].lexeme;
                            }
                        }
                    }
                    j++;
                }

                for (const auto& m : hospitalData->medicos) {
                    if (m.nombre == c.medico) {
                        c.especialidad = m.especialidad;
                        break;
                    }
                }
                hospitalData->citas.push_back(c);
            }
        }

        // Parsear DIAGNOSTICO
        if (t.type == TokenType::DIAGNOSTICO && ctx == Context::DIAGNOSTICOS) {
            if (i + 2 < tokens.size() && tokens[i+2].type == TokenType::STRING) {
                std::string nombrePaciente = tokens[i+2].lexeme.substr(1, tokens[i+2].lexeme.length()-2);

                // Buscar el paciente
                for (auto& p : hospitalData->pacientes) {
                    if (p.nombre == nombrePaciente) {
                        p.tieneDiagnostico = true;

                        // Crear nuevo diagnóstico
                        DiagnosticoInfo diag;

                        size_t j = i + 3;
                        while (j < tokens.size() && tokens[j].type != TokenType::RBRACKET) {
                            if (tokens[j].type == TokenType::CONDICION) {
                                if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                                    if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::STRING) {
                                        diag.condicion = tokens[j+2].lexeme.substr(1, tokens[j+2].lexeme.length()-2);
                                    }
                                }
                            }
                            else if (tokens[j].type == TokenType::MEDICAMENTO) {
                                if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                                    if (j + 2 < tokens.size() && tokens[j+2].type == TokenType::STRING) {
                                        diag.medicamento = tokens[j+2].lexeme.substr(1, tokens[j+2].lexeme.length()-2);
                                        hospitalData->medicamentoFrecuencia[diag.medicamento]++;
                                    }
                                }
                            }
                            else if (tokens[j].type == TokenType::DOSIS) {
                                if (j + 1 < tokens.size() && tokens[j+1].type == TokenType::COLON) {
                                    if (j + 2 < tokens.size()) {
                                        TokenType dosisType = tokens[j+2].type;
                                        if (dosisType == TokenType::DIARIA || dosisType == TokenType::CADA_8_HORAS ||
                                            dosisType == TokenType::CADA_12_HORAS || dosisType == TokenType::SEMANAL) {
                                            diag.dosis = Token::typeToString(dosisType);
                                        }
                                    }
                                }
                            }
                            j++;
                        }

                        // Agregar diagnóstico a la lista
                        p.diagnosticos.push_back(diag);
                        break;
                    }
                }
            }
        }
    }

    hospitalData->calcularEstadisticas();
    hospitalData->detectarConflictosHorario();
}

void MainWindow::generateAllReports() {
    QString outputDir = QDir::currentPath() + "/bin/reportes/";
    QDir().mkpath(outputDir);

    QFile f1(outputDir + "reporte1_pacientes.html");
    if (f1.open(QIODevice::WriteOnly)) {
        f1.write(ReportGenerator::generatePatientReport(hospitalData).c_str());
        f1.close();
    }

    QFile f2(outputDir + "reporte2_medicos.html");
    if (f2.open(QIODevice::WriteOnly)) {
        f2.write(ReportGenerator::generateDoctorWorkloadReport(hospitalData).c_str());
        f2.close();
    }

    QFile f3(outputDir + "reporte3_citas.html");
    if (f3.open(QIODevice::WriteOnly)) {
        f3.write(ReportGenerator::generateAppointmentReport(hospitalData).c_str());
        f3.close();
    }

    QFile f4(outputDir + "reporte4_estadisticas.html");
    if (f4.open(QIODevice::WriteOnly)) {
        f4.write(ReportGenerator::generateStatisticsReport(hospitalData).c_str());
        f4.close();
    }

    QFile fd(outputDir + "hospital.dot");
    if (fd.open(QIODevice::WriteOnly)) {
        fd.write(ReportGenerator::generateDotGraph(hospitalData).c_str());
        fd.close();
    }

    if (errorManager->hasErrors()) {
        QFile fe(outputDir + "errores.html");
        if (fe.open(QIODevice::WriteOnly)) {
            fe.write(ReportGenerator::generateErrorReportHTML(*errorManager).c_str());
            fe.close();
        }
    }

    QFile fst(outputDir + "estadisticas_tokens.html");
    if (fst.open(QIODevice::WriteOnly)) {
        fst.write(ReportGenerator::generateTokenStatsReport(lastStats.tokenFrequency, lastStats.analysisTimeMs).c_str());
        fst.close();
    }
}

void MainWindow::generarImagenGraphviz() {
    QString dotPath = QDir::currentPath() + "/bin/reportes/hospital.dot";
    QString pngPath = QDir::currentPath() + "/bin/reportes/hospital.png";

    // Ejecutar comando dot de Graphviz
    QProcess process;
    process.start("dot", QStringList() << "-Tpng" << dotPath << "-o" << pngPath);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        QMessageBox::information(this, "Éxito", "Diagrama generado: hospital.png");
    } else {
        QMessageBox::warning(this, "Error", "No se pudo generar la imagen. ¿Está instalado Graphviz en el PATH?");
    }
}

void MainWindow::showReports() {
    QString outputDir = QDir::currentPath() + "/bin/reportes/";
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "reporte1_pacientes.html"));
}

void MainWindow::showTokenStats() {
    QString outputDir = QDir::currentPath() + "/bin/reportes/";
    QDesktopServices::openUrl(QUrl::fromLocalFile(outputDir + "estadisticas_tokens.html"));
}
bool MainWindow::isEspecialidad(TokenType t) {
    return t == TokenType::CARDIOLOGIA ||
           t == TokenType::NEUROLOGIA ||
           t == TokenType::PEDIATRIA ||
           t == TokenType::CIRUGIA ||
           t == TokenType::MEDICINA_GENERAL ||
           t == TokenType::ONCOLOGIA;
}

bool MainWindow::isDosis(TokenType t) {
    return t == TokenType::DIARIA ||
           t == TokenType::CADA_8_HORAS ||
           t == TokenType::CADA_12_HORAS ||
           t == TokenType::SEMANAL;
}