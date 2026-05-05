#include "reportgenerator.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

ReportGenerator::ReportGenerator(const Tablero& tab, const std::vector<Token>& tok,
                                 const ErrorManager& err, const std::shared_ptr<ASTNode>& r)
    : tablero(tab), tokens(tok), errorMgr(err), root(r), dotCounter(0) {}

std::string ReportGenerator::getPriorityColor(const std::string& p) const {
    if (p == "ALTA") return "#e74c3c";
    if (p == "MEDIA") return "#f1c40f";
    if (p == "BAJA") return "#2ecc71";
    return "#95a5a6";
}

// REPORTE 1: Tablero Kanban Visual
void ReportGenerator::generateKanbanReport(const std::string& filename) {
    std::ofstream file(filename);

    file << R"(<!DOCTYPE html>
<html><head><meta charset="UTF-8">
<title>Tablero Kanban - )" << tablero.nombre << R"(</title>
<style>
body{font-family:'Segoe UI',Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);padding:20px;margin:0;min-height:100vh}
h1{color:white;text-align:center;text-shadow:2px 2px 4px rgba(0,0,0,0.3);font-size:2.5em;margin-bottom:30px}
.error-banner{background:#e74c3c;color:white;padding:15px 25px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold;font-size:16px}
.warning-banner{background:#f39c12;color:white;padding:15px 25px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold;font-size:16px}
.board{display:flex;gap:25px;justify-content:center;flex-wrap:wrap;align-items:flex-start}
.column{background:rgba(255,255,255,0.95);border-radius:12px;width:320px;padding:20px;box-shadow:0 8px 32px rgba(0,0,0,0.2);backdrop-filter:blur(10px)}
.column h2{margin-top:0;color:#2c3e50;font-size:20px;border-bottom:3px solid #3498db;padding-bottom:12px;display:flex;justify-content:space-between;align-items:center}
.badge-count{background:#3498db;color:white;padding:4px 12px;border-radius:20px;font-size:14px}
.card{background:white;border-radius:10px;padding:16px;margin-bottom:12px;box-shadow:0 2px 8px rgba(0,0,0,0.08);border-left:5px solid #3498db;transition:transform 0.2s}
.card:hover{transform:translateY(-3px);box-shadow:0 4px 16px rgba(0,0,0,0.15)}
.card h3{margin:0 0 10px 0;font-size:17px;color:#2c3e50}
.priority-badge{display:inline-block;padding:5px 10px;border-radius:6px;font-size:12px;font-weight:bold;color:white;margin-bottom:8px}
.info{color:#5d6d7e;font-size:14px;margin:5px 0;display:flex;align-items:center;gap:8px}
.icon{font-size:16px}
.footer{text-align:center;color:rgba(255,255,255,0.8);margin-top:40px;font-size:14px}
.empty-state{text-align:center;color:#7f8c8d;padding:40px;font-style:italic}
</style></head><body>
<h1>📋 )" << tablero.nombre << R"(</h1>)";

    // Banner de error si hay errores
    if (errorMgr.hasErrors()) {
        file << R"(<div class="error-banner">⚠️ Este reporte fue generado con errores en el análisis. Algunos datos pueden estar incompletos.</div>)";
    } else {
        file << R"(<div class="warning-banner">✅ Análisis completado sin errores</div>)";
    }

    file << R"(<div class="board">)";

    if (tablero.columnas.empty()) {
        file << R"(<div class="empty-state">No se encontraron columnas válidas en el archivo.</div>)";
    }

    for (const auto& col : tablero.columnas) {
        file << R"(<div class="column"><h2>)" << col.nombre
             << R"(<span class="badge-count">)" << col.tareas.size() << " tareas</span></h2>";

        if (col.tareas.empty()) {
            file << R"(<div class="empty-state">Sin tareas en esta columna</div>)";
        }

        for (const auto& tar : col.tareas) {
            std::string prio = tar.getPrioridad();
            std::string color = getPriorityColor(prio);
            file << R"(<div class="card" style="border-left-color:)" << color << "\">"
                 << R"(<span class="priority-badge" style="background:)" << color << "\">" << prio << "</span>"
                 << R"(<h3>)" << tar.nombre << "</h3>"
                 << R"(<div class="info"><span class="icon">👤</span>)" << tar.getResponsable() << "</div>"
                 << R"(<div class="info"><span class="icon">📅</span>)" << tar.getFechaLimite() << "</div></div>";
        }
        file << "</div>";
    }

    file << R"(</div><div class="footer">Generado por TaskScript Analyzer - LFP Proyecto 2</div></body></html>)";
    file.close();
}

// REPORTE 2: Carga por Responsable
void ReportGenerator::generateResponsableReport(const std::string& filename) {
    std::map<std::string, std::map<std::string, int>> stats;
    int totalTareas = 0;

    for (const auto& col : tablero.columnas) {
        for (const auto& tar : col.tareas) {
            std::string resp = tar.getResponsable();
            if (resp.empty()) continue; // Saltar tareas sin responsable
            std::string prio = tar.getPrioridad();
            stats[resp][prio]++;
            stats[resp]["TOTAL"]++;
            totalTareas++;
        }
    }

    std::ofstream file(filename);
    file << R"(<!DOCTYPE html>
<html><head><meta charset="UTF-8">
<title>Carga por Responsable - )" << tablero.nombre << R"(</title>
<style>
body{font-family:'Segoe UI',Arial,sans-serif;background:#f8f9fa;padding:20px}
h1{color:#2c3e50;text-align:center;margin-bottom:10px}
.subtitle{text-align:center;color:#7f8c8d;margin-bottom:30px}
.error-banner{background:#e74c3c;color:white;padding:15px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold}
.warning-banner{background:#27ae60;color:white;padding:15px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold}
table{width:85%;margin:0 auto;border-collapse:separate;border-spacing:0;background:white;box-shadow:0 4px 20px rgba(0,0,0,0.1);border-radius:12px;overflow:hidden}
th,td{padding:15px;text-align:left}
th{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;font-weight:600;text-transform:uppercase;font-size:13px;letter-spacing:1px}
tr:not(:last-child) td{border-bottom:1px solid #ecf0f1}
tr:hover{background:#f8f9fa}
td{font-size:14px;color:#2c3e50}
.resp-name{font-weight:600;color:#2c3e50;font-size:15px}
.total-badge{background:#3498db;color:white;padding:6px 14px;border-radius:20px;font-weight:bold;font-size:14px}
.prio-alta{color:#e74c3c;font-weight:bold;background:#fdf2f2;padding:4px 10px;border-radius:6px}
.prio-media{color:#f39c12;font-weight:bold;background:#fef9e7;padding:4px 10px;border-radius:6px}
.prio-baja{color:#27ae60;font-weight:bold;background:#eafaf1;padding:4px 10px;border-radius:6px}
.bar-container{width:100%;background:#ecf0f1;border-radius:10px;height:24px;overflow:hidden;position:relative}
.bar{height:100%;background:linear-gradient(90deg,#667eea 0%,#764ba2 100%);text-align:center;color:white;line-height:24px;font-size:12px;font-weight:bold;transition:width 1s ease}
.empty-state{text-align:center;color:#7f8c8d;padding:60px;font-size:16px}
.summary{text-align:center;margin-top:30px;padding:20px;background:white;border-radius:12px;box-shadow:0 2px 10px rgba(0,0,0,0.05);width:85%;margin-left:auto;margin-right:auto}
.summary h3{margin-top:0;color:#2c3e50}
</style></head><body>
<h1>👥 Carga por Responsable</h1>
<p class="subtitle">)" << tablero.nombre << "</p>";

    // Banner
    if (errorMgr.hasErrors()) {
        file << R"(<div class="error-banner">⚠️ Este reporte fue generado con errores en el análisis. Algunos datos pueden estar incompletos.</div>)";
    } else {
        file << R"(<div class="warning-banner">✅ Análisis completado sin errores</div>)";
    }

    if (stats.empty()) {
        file << R"(<div class="empty-state">No se encontraron responsables válidos en las tareas.</div>)";
    } else {
        file << R"(<table><thead><tr><th>Responsable</th><th>Total Tareas</th><th>🔴 Alta</th><th>🟡 Media</th><th>🟢 Baja</th><th>Distribución</th></tr></thead><tbody>)";

        for (const auto& [resp, data] : stats) {
            int total = data.at("TOTAL");
            int alta = data.count("ALTA") ? data.at("ALTA") : 0;
            int media = data.count("MEDIA") ? data.at("MEDIA") : 0;
            int baja = data.count("BAJA") ? data.at("BAJA") : 0;
            double pct = totalTareas > 0 ? (total * 100.0 / totalTareas) : 0;

            file << "<tr><td class='resp-name'>" << resp
                 << "</td><td><span class='total-badge'>" << total << "</span></td>"
                 << "<td class='prio-alta'>" << alta << "</td>"
                 << "<td class='prio-media'>" << media << "</td>"
                 << "<td class='prio-baja'>" << baja << "</td>"
                 << "<td><div class='bar-container'><div class='bar' style='width:"
                 << pct << "%'>" << std::fixed << std::setprecision(1) << pct << "%</div></div></td></tr>";
        }
        file << "</tbody></table>";
    }

    file << R"(<div class="summary"><h3>📊 Resumen del Tablero</h3>
<p><strong>Total de tareas:</strong> )" << totalTareas
         << " | <strong>Responsables:</strong> " << stats.size()
         << "</p></div>";

    file << "</body></html>";
    file.close();
}

// REPORTE 3: Resumen por Columna (Tareas por Estado)
void ReportGenerator::generateColumnasReport(const std::string& filename) {
    int totalTareasGlobal = 0;
    std::map<std::string, std::map<std::string, int>> colStats;

    for (const auto& col : tablero.columnas) {
        std::map<std::string, int>& stats = colStats[col.nombre];
        for (const auto& tar : col.tareas) {
            stats[tar.getPrioridad()]++;
            stats["TOTAL"]++;
            totalTareasGlobal++;
        }
    }

    std::ofstream file(filename);
    file << R"(<!DOCTYPE html>
<html><head><meta charset="UTF-8">
<title>Resumen por Columna - )" << tablero.nombre << R"(</title>
<style>
body{font-family:'Segoe UI',Arial,sans-serif;background:#f0f2f5;padding:20px}
h1{color:#2c3e50;text-align:center;margin-bottom:10px}
.subtitle{text-align:center;color:#7f8c8d;margin-bottom:40px}
.error-banner{background:#e74c3c;color:white;padding:15px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold;max-width:800px;margin-left:auto;margin-right:auto}
.warning-banner{background:#27ae60;color:white;padding:15px;border-radius:8px;text-align:center;margin-bottom:25px;font-weight:bold;max-width:800px;margin-left:auto;margin-right:auto}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(350px,1fr));gap:25px;max-width:1200px;margin:0 auto}
.card{background:white;border-radius:16px;padding:25px;box-shadow:0 4px 20px rgba(0,0,0,0.08);position:relative;overflow:hidden}
.card::before{content:'';position:absolute;top:0;left:0;width:100%;height:5px;background:linear-gradient(90deg,#667eea 0%,#764ba2 100%)}
.card h2{margin-top:0;color:#2c3e50;font-size:22px;display:flex;justify-content:space-between;align-items:center}
.task-count{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:white;padding:8px 16px;border-radius:25px;font-size:16px;font-weight:bold}
.stats-grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:15px;margin-top:20px}
.stat-box{text-align:center;padding:15px;border-radius:10px;background:#f8f9fa}
.stat-box.alta{background:#fdf2f2}
.stat-box.media{background:#fef9e7}
.stat-box.baja{background:#eafaf1}
.stat-number{font-size:28px;font-weight:bold;margin-bottom:5px}
.stat-box.alta .stat-number{color:#e74c3c}
.stat-box.media .stat-number{color:#f39c12}
.stat-box.baja .stat-number{color:#27ae60}
.stat-label{font-size:12px;color:#7f8c8d;text-transform:uppercase;letter-spacing:1px}
.progress-section{margin-top:20px}
.progress-label{display:flex;justify-content:space-between;margin-bottom:8px;font-size:14px;color:#5d6d7e}
.progress-bar{width:100%;height:20px;background:#ecf0f1;border-radius:10px;overflow:hidden}
.progress-fill{height:100%;background:linear-gradient(90deg,#667eea 0%,#764ba2 100%);border-radius:10px;transition:width 0.5s ease}
.empty-state{text-align:center;color:#7f8c8d;padding:60px;font-size:16px;grid-column:1/-1}
.summary-box{max-width:800px;margin:40px auto;background:white;padding:30px;border-radius:16px;box-shadow:0 4px 20px rgba(0,0,0,0.08);text-align:center}
.summary-box h2{color:#2c3e50;margin-top:0}
.big-number{font-size:48px;font-weight:bold;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);-webkit-background-clip:text;-webkit-text-fill-color:transparent}
</style></head><body>
<h1>📊 Resumen por Columna</h1>
<p class="subtitle">)" << tablero.nombre << "</p>";

    // Banner
    if (errorMgr.hasErrors()) {
        file << R"(<div class="error-banner">⚠️ Este reporte fue generado con errores en el análisis. Algunos datos pueden estar incompletos.</div>)";
    } else {
        file << R"(<div class="warning-banner">✅ Análisis completado sin errores</div>)";
    }

    file << R"(<div class="grid">)";

    if (tablero.columnas.empty()) {
        file << R"(<div class="empty-state">No se encontraron columnas válidas en el archivo.</div>)";
    }

    for (const auto& col : tablero.columnas) {
        const auto& stats = colStats[col.nombre];
        int total = stats.count("TOTAL") ? stats.at("TOTAL") : 0;
        int alta = stats.count("ALTA") ? stats.at("ALTA") : 0;
        int media = stats.count("MEDIA") ? stats.at("MEDIA") : 0;
        int baja = stats.count("BAJA") ? stats.at("BAJA") : 0;

        double pctGlobal = totalTareasGlobal > 0 ? (total * 100.0 / totalTareasGlobal) : 0;

        file << R"(<div class="card">
<h2>)" << col.nombre << R"(<span class="task-count">)" << total << R"( tareas</span></h2>
<div class="stats-grid">
    <div class="stat-box alta">
        <div class="stat-number">)" << alta << R"(</div>
        <div class="stat-label">Alta</div>
    </div>
    <div class="stat-box media">
        <div class="stat-number">)" << media << R"(</div>
        <div class="stat-label">Media</div>
    </div>
    <div class="stat-box baja">
        <div class="stat-number">)" << baja << R"(</div>
        <div class="stat-label">Baja</div>
    </div>
</div>
<div class="progress-section">
    <div class="progress-label"><span>Porcentaje del tablero</span><span>)"
             << std::fixed << std::setprecision(1) << pctGlobal << R"(%</span></div>
    <div class="progress-bar"><div class="progress-fill" style="width:)" << pctGlobal << R"(%"></div></div>
</div>
</div>)";
    }

    file << R"(</div>
<div class="summary-box">
    <h2>📈 Totales del Proyecto</h2>
    <div class="big-number">)" << totalTareasGlobal << R"(</div>
    <p style="color:#7f8c8d;font-size:16px">tareas en )" << tablero.columnas.size() << R"( columnas</p>
</div>
</body></html>)";

    file.close();
}

// REPORTE AUXILIAR: Tabla de Tokens y Errores (para visualización/debug)
void ReportGenerator::generateErrorsReport(const std::string& filename) {
    std::ofstream file(filename);
    file << R"(<!DOCTYPE html>
<html><head><meta charset="UTF-8">
<title>Tabla de Tokens y Errores - )" << tablero.nombre << R"(</title>
<style>
body{font-family:'Segoe UI',Arial,sans-serif;padding:20px;background:#f5f5f5}
h1{color:#2c3e50;text-align:center}
h2{color:#34495e;margin-top:40px;border-bottom:2px solid #3498db;padding-bottom:10px}
table{width:95%;margin:20px auto;border-collapse:separate;border-spacing:0;background:white;box-shadow:0 2px 10px rgba(0,0,0,0.1);border-radius:8px;overflow:hidden}
th,td{padding:12px;text-align:center}
th{background:#2980b9;color:white;font-weight:600}
tr:not(:last-child) td{border-bottom:1px solid #ecf0f1}
tr:hover{background:#f8f9fa}
.token-type{color:#2980b9;font-weight:bold}
.error-type{color:#e74c3c;font-weight:bold}
.ok-msg{color:#27ae60;text-align:center;font-size:18px;padding:40px}
</style></head><body>
<h1>🔍 Análisis Léxico y Sintáctico</h1>)";

    // Tabla de Tokens
    file << "<h2>📋 Tabla de Tokens</h2><table><thead><tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Línea</th><th>Columna</th></tr></thead><tbody>";

    int i = 1;
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::EOF_TOKEN) continue;
        file << "<tr><td>" << i++ << "</td><td>" << tok.lexeme
             << "</td><td class='token-type'>" << Token::typeToString(tok.type)
             << "</td><td>" << tok.line << "</td><td>" << tok.column << "</td></tr>";
    }
    file << "</tbody></table>";

    // Tabla de Errores
    file << "<h2>⚠️ Tabla de Errores</h2>";
    if (errorMgr.hasErrors()) {
        file << "<table><thead><tr><th>#</th><th>Lexema</th><th>Tipo</th><th>Descripción</th><th>Línea</th><th>Columna</th><th>Severidad</th></tr></thead><tbody>";
        for (const auto& err : errorMgr.getErrors()) {
            file << "<tr><td>" << err.number << "</td><td>" << err.lexeme
                 << "</td><td class='error-type'>" << err.type << "</td><td>" << err.description
                 << "</td><td>" << err.line << "</td><td>" << err.column
                 << "</td><td>" << err.severity << "</td></tr>";
        }
        file << "</tbody></table>";
    } else {
        file << "<p class='ok-msg'>✅ No se encontraron errores en el análisis.</p>";
    }

    file << "</body></html>";
    file.close();
}

// GRAPHVIZ DOT: Árbol de Derivación
void ReportGenerator::generateDOT(const std::string& filename) {
    std::ofstream file(filename);
    file << "digraph ArbolDerivacion {\n";
    file << "rankdir=TB;\n";
    file << "node [shape=box,style=filled,fontname=\"Arial\"];\n";
    file << "edge [arrowhead=none];\n";

    int counter = 0;
    generateDOTNode(file, root, counter);

    file << "}\n";
    file.close();
}

void ReportGenerator::generateDOTNode(std::ofstream& out, std::shared_ptr<ASTNode> node, int& counter) {
    int myId = counter++;
    std::string label = node->isTerminal ? node->value : node->label;

    // Escapar comillas dobles para DOT
    size_t pos = 0;
    while ((pos = label.find('"', pos)) != std::string::npos) {
        label.replace(pos, 1, "\\\"");
        pos += 2;
    }

    std::string color = node->isTerminal ? "#D6EAF8" : "#2E75B6";
    std::string fontcolor = node->isTerminal ? "black" : "white";
    std::string shape = node->isTerminal ? "ellipse" : "box";

    out << "n" << myId << "[label=\"" << label << "\",fillcolor=\""
        << color << "\",fontcolor=" << fontcolor << ",shape=" << shape << "];\n";

    for (auto& child : node->children) {
        int childId = counter;
        generateDOTNode(out, child, counter);
        out << "n" << myId << "->n" << childId << ";\n";
    }
}