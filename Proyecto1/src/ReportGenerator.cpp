#include "ReportGenerator.h"
#include "ErrorManager.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::string ReportGenerator::generatePatientReport(const HospitalData* data) {
    std::stringstream html;
    html << "<html><head><style>"
         << "body{font-family:Arial,sans-serif;margin:20px;background:#f5f5f5;}"
         << "h1{color:#2c3e50;text-align:center;}"
         << "table{width:100%;border-collapse:collapse;background:white;box-shadow:0 2px 4px rgba(0,0,0,0.1);}"
         << "th{background:#34495e;color:white;padding:12px;text-align:left;}"
         << "td{padding:10px;border-bottom:1px solid #ddd;}"
         << ".activo{color:green;font-weight:bold;}"
         << ".sindiag{color:orange;font-weight:bold;}"
         << ".critico{color:red;font-weight:bold;background:#fadbd8;}"
         << "tr:hover{background:#f9f9f9;}"
         << "</style></head><body>"
         << "<h1>Reporte 1 - Historial de Pacientes</h1>"
         << "<table><tr><th>Paciente</th><th>Edad</th><th>Sangre</th>"
         << "<th>Diagnostico Activo</th><th>Medicamento/Dosis</th><th>Estado</th></tr>";

    for (const auto& p : data->pacientes) {
        std::string estadoClass, estadoText;

        if (!p.tieneDiagnostico || p.diagnosticos.empty()) {
            estadoClass = "sindiag";
            estadoText = "SIN DIAG.";
        }
        else if (p.esCritico()) {
            estadoClass = "critico";
            estadoText = "CRITICO";
        }
        else {
            estadoClass = "activo";
            estadoText = "ACTIVO";
        }

        std::string condicion, medicamento;
        if (p.diagnosticos.empty()) {
            condicion = "Sin diagnostico";
            medicamento = "-";
        } else {
            condicion = p.getCondicionActiva();
            medicamento = p.getMedicamentoActivo();
        }

        html << "<tr><td>" << p.nombre << "</td>"
             << "<td>" << p.edad << "</td>"
             << "<td>" << p.tipoSangre << "</td>"
             << "<td>" << condicion << "</td>"
             << "<td>" << medicamento << "</td>"
             << "<td class='" << estadoClass << "'>" << estadoText << "</td></tr>";
    }

    html << "</table></body></html>";
    return html.str();
}

std::string ReportGenerator::generateDoctorWorkloadReport(const HospitalData* data) {
    std::stringstream html;
    html << "<html><head><style>"
         << "body{font-family:Arial,sans-serif;margin:20px;background:#f5f5f5;}"
         << "h1{color:#2c3e50;text-align:center;}"
         << "table{width:100%;border-collapse:collapse;background:white;}"
         << "th{background:#2980b9;color:white;padding:12px;text-align:left;}"
         << "td{padding:10px;border-bottom:1px solid #ddd;}"
         << ".baja{color:#3498db;font-weight:bold;}.normal{color:#27ae60;font-weight:bold;}"
         << ".alta{color:#f39c12;font-weight:bold;}.saturada{color:#e74c3c;font-weight:bold;}"
         << "</style></head><body>"
         << "<h1>Reporte 2 - Carga de Medicos por Especialidad</h1>"
         << "<table><tr><th>Medico</th><th>Codigo</th><th>Especialidad</th><th>Citas Prog.</th><th>Pacientes</th><th>Nivel de Carga</th></tr>";

    for (const auto& m : data->medicos) {
        int carga = m.citasProgramadas;
        std::string nivelClass, nivelText;
        if (carga <= 3) { nivelClass = "baja"; nivelText = "BAJA"; }
        else if (carga <= 6) { nivelClass = "normal"; nivelText = "NORMAL"; }
        else if (carga <= 8) { nivelClass = "alta"; nivelText = "ALTA"; }
        else { nivelClass = "saturada"; nivelText = "SATURADA"; }

        html << "<tr><td>" << m.nombre << "</td><td>" << m.codigo << "</td><td>" << m.especialidad << "</td>"
             << "<td>" << m.citasProgramadas << "</td><td>" << m.pacientesAtendidos.size() << "</td>"
             << "<td class='" << nivelClass << "'>" << nivelText << "</td></tr>";
    }
    html << "</table></body></html>";
    return html.str();
}

std::string ReportGenerator::generateAppointmentReport(const HospitalData* data) {
    std::stringstream html;
    html << "<html><head><style>"
         << "body{font-family:Arial,sans-serif;margin:20px;background:#f5f5f5;}"
         << "h1{color:#2c3e50;text-align:center;}"
         << "table{width:100%;border-collapse:collapse;background:white;}"
         << "th{background:#16a085;color:white;padding:12px;text-align:left;}"
         << "td{padding:10px;border-bottom:1px solid #ddd;}"
         << ".confirmada{background:#d5f4e6;}.conflicto{background:#fadbd8;color:#c0392b;font-weight:bold;}"
         << "</style></head><body>"
         << "<h1>Reporte 3 - Agenda de Citas con Deteccion de Conflictos</h1>"
         << "<table><tr><th>Fecha</th><th>Hora</th><th>Paciente</th><th>Medico</th><th>Especialidad</th><th>Estado</th></tr>";

    for (const auto& c : data->citas) {
        std::string rowClass = c.conflicto ? "conflicto" : "confirmada";
        std::string estado = c.conflicto ? "CONFLICTO" : "CONFIRMADA";

        html << "<tr class='" << rowClass << "'><td>" << c.fecha << "</td><td>" << c.hora << "</td>"
             << "<td>" << c.paciente << "</td><td>" << c.medico << "</td><td>" << c.especialidad << "</td>"
             << "<td>" << estado << "</td></tr>";
    }
    html << "</table></body></html>";
    return html.str();
}

std::string ReportGenerator::generateStatisticsReport(const HospitalData* data) {
    std::stringstream html;

    int totalPacientes = data->pacientes.size();
    int totalMedicos = data->medicos.size();
    int totalCitas = data->citas.size();
    int citasConflicto = 0;
    for (const auto& c : data->citas) if (c.conflicto) citasConflicto++;
    int pacientesConDiag = 0;
    for (const auto& p : data->pacientes) if (p.tieneDiagnostico) pacientesConDiag++;

    std::string medMasFrecuente = "N/A";
    int maxFreq = 0;
    for (const auto& pair : data->medicamentoFrecuencia) {
        if (pair.second > maxFreq) { maxFreq = pair.second; medMasFrecuente = pair.first; }
    }

    std::map<std::string, int> cargaPorEspecialidad;
    for (const auto& m : data->medicos) cargaPorEspecialidad[m.especialidad] += m.citasProgramadas;

    std::string espMayorCarga = "N/A";
    int maxCarga = 0;
    for (const auto& pair : cargaPorEspecialidad) {
        if (pair.second > maxCarga) { maxCarga = pair.second; espMayorCarga = pair.first; }
    }

    double promedioEdad = 0;
    for (const auto& p : data->pacientes) promedioEdad += p.edad;
    if (totalPacientes > 0) promedioEdad /= totalPacientes;

    html << "<html><head><style>"
         << "body{font-family:Arial,sans-serif;margin:20px;background:#f5f5f5;}"
         << "h1,h2{color:#2c3e50;}"
         << ".kpi-table,.dist-table{width:100%;border-collapse:collapse;margin:20px 0;background:white;}"
         << "th{background:#8e44ad;color:white;padding:12px;text-align:left;}"
         << "td{padding:10px;border-bottom:1px solid #ddd;}"
         << ".bar-bg{background:#ecf0f1;width:100%;height:20px;border-radius:10px;}"
         << ".bar-fill{height:100%;background:#3498db;border-radius:10px;text-align:center;color:white;line-height:20px;font-size:12px;}"
         << ".high-usage .bar-fill{background:#e74c3c;}"
         << "</style></head><body>"
         << "<h1>Reporte 4 - Estadistico General del Hospital</h1>"
         << "<h2>Seccion A - Indicadores Clave</h2>"
         << "<table class='kpi-table'><tr><th>Indicador</th><th>Valor</th></tr>"
         << "<tr><td>Nombre del hospital</td><td>" << (data->nombre.empty() ? "Hospital General" : data->nombre) << "</td></tr>"
         << "<tr><td>Total de pacientes</td><td>" << totalPacientes << "</td></tr>"
         << "<tr><td>Total de medicos</td><td>" << totalMedicos << "</td></tr>"
         << "<tr><td>Total de citas</td><td>" << totalCitas << "</td></tr>"
         << "<tr><td>Citas con conflicto</td><td>" << citasConflicto << "</td></tr>"
         << "<tr><td>Pacientes con diagnostico</td><td>" << pacientesConDiag << " de " << totalPacientes
         << " (" << std::fixed << std::setprecision(1) << (totalPacientes > 0 ? 100.0 * pacientesConDiag / totalPacientes : 0) << "%)</td></tr>"
         << "<tr><td>Medicamento mas prescrito</td><td>" << medMasFrecuente << " (" << maxFreq << " pacientes)</td></tr>"
         << "<tr><td>Especialidad con mayor carga</td><td>" << espMayorCarga << " (" << maxCarga << " citas)</td></tr>"
         << "<tr><td>Promedio de edad</td><td>" << std::fixed << std::setprecision(1) << promedioEdad << " anos</td></tr>"
         << "</table>";

    html << "<h2>Seccion B - Distribucion por Especialidad</h2>"
         << "<table class='dist-table'><tr><th>Especialidad</th><th>Medicos</th><th>Citas</th><th>Pacientes</th><th>Ocupacion</th></tr>";

    for (const auto& pair : cargaPorEspecialidad) {
        int numMedicos = 0, pacientesEsp = 0;
        for (const auto& m : data->medicos) {
            if (m.especialidad == pair.first) {
                numMedicos++;
                pacientesEsp += m.pacientesAtendidos.size();
            }
        }
        double pct = totalCitas > 0 ? (100.0 * pair.second / totalCitas) : 0;
        bool alta = pct > 80;

        html << "<tr" << (alta ? " class='high-usage'" : "") << "><td>" << pair.first << "</td><td>" << numMedicos
             << "</td><td>" << pair.second << "</td><td>" << pacientesEsp << "</td>"
             << "<td><div class='bar-bg'><div class='bar-fill' style='width:" << pct << "%'>" << (int)pct << "%</div></div></td></tr>";
    }
    html << "</table></body></html>";
    return html.str();
}

std::string ReportGenerator::generateDotGraph(const HospitalData* data) {
    std::stringstream dot;
    dot << "digraph Hospital {\nrankdir=TB;\nnode [shape=box,style=filled,fontname=\"Arial\"];\n\n"
        << "H [label=\"" << (data->nombre.empty() ? "Hospital General" : data->nombre)
        << "\",fillcolor=\"#1A4731\",fontcolor=white,shape=ellipse];\n"
        << "P [label=\"PACIENTES\",fillcolor=\"#2E7D52\",fontcolor=white];\n"
        << "M [label=\"MEDICOS\",fillcolor=\"#2E7D52\",fontcolor=white];\n"
        << "C [label=\"CITAS\",fillcolor=\"#2E7D52\",fontcolor=white];\n"
        << "D [label=\"DIAGNOSTICOS\",fillcolor=\"#2E7D52\",fontcolor=white];\n"
        << "H -> P; H -> M; H -> C; H -> D;\n\n";

    // Nodos pacientes
    for (size_t i = 0; i < data->pacientes.size(); ++i) {
        const auto& p = data->pacientes[i];
        dot << "p" << i << " [label=\"" << p.nombre << "\\n" << p.tipoSangre << "|Hab." << p.habitacion
            << "\",fillcolor=\"#D4EDDA\"];\nP -> p" << i << ";\n";
    }

    // Nodos médicos
    for (size_t i = 0; i < data->medicos.size(); ++i) {
        const auto& m = data->medicos[i];
        dot << "m" << i << " [label=\"" << m.nombre << "\\n" << m.codigo << "|" << m.especialidad
            << "\",fillcolor=\"#D6EAF8\"];\nM -> m" << i << ";\n";
    }

    // Aristas citas (paciente -> médico)
    for (size_t i = 0; i < data->citas.size(); ++i) {
        const auto& c = data->citas[i];
        int pIdx = -1, mIdx = -1;
        for (size_t j = 0; j < data->pacientes.size(); ++j)
            if (data->pacientes[j].nombre == c.paciente) pIdx = j;
        for (size_t j = 0; j < data->medicos.size(); ++j)
            if (data->medicos[j].nombre == c.medico) mIdx = j;

        if (pIdx >= 0 && mIdx >= 0) {
            dot << "p" << pIdx << " -> m" << mIdx
                << " [label=\"" << c.fecha << " " << c.hora << "\",color=\"#E67E22\",style=dashed];\n";
        }
    }

    // Nodos diagnósticos
    int dCount = 0;
    for (size_t i = 0; i < data->pacientes.size(); ++i) {
        const auto& p = data->pacientes[i];
        // Crear un nodo por CADA diagnóstico del paciente
        for (const auto& diag : p.diagnosticos) {
            dot << "d" << dCount << " [label=\"" << diag.condicion << "\\n"
                << diag.medicamento << " / " << diag.dosis
                << "\",fillcolor=\"#FDEBD0\"];\n"
                << "D -> d" << dCount << ";\n"
                << "d" << dCount << " -> p" << i << " [label=\"diagnostico\",color=\"#C0392B\"];\n";
            dCount++;
        }
    }

    dot << "}\n";
    return dot.str();
}

std::string ReportGenerator::generateTokenStatsReport(const std::map<TokenType, int>& freq, double timeMs) {
    std::stringstream html;
    html << "<html><head><style>"
         << "body{font-family:Arial,sans-serif;margin:20px;background:#f5f5f5;}"
         << "h1{color:#2c3e50;}"
         << ".stats-box{background:white;padding:20px;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,0.1);margin-bottom:20px;}"
         << "table{width:100%;border-collapse:collapse;}"
         << "th{background:#2c3e50;color:white;padding:10px;text-align:left;}"
         << "td{padding:8px;border-bottom:1px solid #ddd;}"
         << "</style></head><body>"
         << "<h1>Estadisticas del Analisis Lexico</h1>"
         << "<div class='stats-box'><h2>Tiempo de Analisis</h2>"
         << "<p style='font-size:24px;color:#27ae60;font-weight:bold;'>"
         << std::fixed << std::setprecision(2) << timeMs << " ms</p></div>"
         << "<div class='stats-box'><h2>Frecuencia de Tokens por Tipo</h2>"
         << "<table><tr><th>Tipo de Token</th><th>Cantidad</th><th>Porcentaje</th></tr>";

    int total = 0;
    for (const auto& pair : freq) total += pair.second;

    for (const auto& pair : freq) {
        double pct = total > 0 ? (100.0 * pair.second / total) : 0;
        html << "<tr><td>" << Token::typeToString(pair.first) << "</td><td>" << pair.second
             << "</td><td>" << std::fixed << std::setprecision(1) << pct << "%</td></tr>";
    }

    html << "</table></div><div class='stats-box'><h2>Resumen</h2>"
         << "<p><strong>Total de tokens:</strong> " << total << "</p></div></body></html>";
    return html.str();
}

std::string ReportGenerator::generateErrorReportHTML(const ErrorManager& em) {
    return em.generateErrorReport();
}