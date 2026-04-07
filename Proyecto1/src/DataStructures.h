#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <string>
#include <vector>
#include <map>
#include <set>

struct DiagnosticoInfo {
    std::string condicion;
    std::string medicamento;
    std::string dosis;
};

struct Paciente {
    std::string nombre;
    int edad = 0;
    std::string tipoSangre;
    int habitacion = 0;
    std::vector<DiagnosticoInfo> diagnosticos;
    bool tieneDiagnostico = false;

    bool esCritico() const {
        return diagnosticos.size() > 1;
    }

    std::string getCondicionActiva() const {
        if (diagnosticos.empty()) return "Sin diagnostico";
        if (diagnosticos.size() == 1) return diagnosticos[0].condicion;
        return diagnosticos[0].condicion + " (+ " + std::to_string(diagnosticos.size() - 1) + " mas)";
    }

    std::string getMedicamentoActivo() const {
        if (diagnosticos.empty()) return "-";
        if (diagnosticos.size() == 1) return diagnosticos[0].medicamento + " / " + diagnosticos[0].dosis;
        return "Multiple medicacion";
    }
};

struct Medico {
    std::string nombre;
    std::string codigo;
    std::string especialidad;
    int citasProgramadas = 0;
    std::set<std::string> pacientesAtendidos;
};

struct Cita {
    std::string fecha;
    std::string hora;
    std::string paciente;
    std::string medico;
    std::string especialidad;
    bool conflicto = false;
};

struct HospitalData {
    std::string nombre;
    std::vector<Paciente> pacientes;
    std::vector<Medico> medicos;
    std::vector<Cita> citas;
    std::map<std::string, int> medicamentoFrecuencia;

    void detectarConflictosHorario();
    void calcularEstadisticas();
};

#endif