#include "DataStructures.h"

void HospitalData::detectarConflictosHorario() {
    for (size_t i = 0; i < citas.size(); ++i) {
        for (size_t j = i + 1; j < citas.size(); ++j) {
            if (citas[i].medico == citas[j].medico &&
                citas[i].fecha == citas[j].fecha &&
                citas[i].hora == citas[j].hora) {
                citas[i].conflicto = true;
                citas[j].conflicto = true;
            }
        }
    }
}

void HospitalData::calcularEstadisticas() {
    for (auto& medico : medicos) {
        medico.citasProgramadas = 0;
        medico.pacientesAtendidos.clear();
    }

    for (const auto& cita : citas) {
        for (auto& medico : medicos) {
            if (medico.nombre == cita.medico) {
                medico.citasProgramadas++;
                medico.pacientesAtendidos.insert(cita.paciente);
            }
        }
    }
}