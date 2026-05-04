#include "kanbandata.h"

std::string Tarea::getPrioridad() const {
    for (const auto& attr : atributos) {
        if (attr.tipo == "prioridad") return attr.valor;
    }
    return "MEDIA"; // default
}

std::string Tarea::getResponsable() const {
    for (const auto& attr : atributos) {
        if (attr.tipo == "responsable") return attr.valor;
    }
    return "";
}

std::string Tarea::getFechaLimite() const {
    for (const auto& attr : atributos) {
        if (attr.tipo == "fecha_limite") return attr.valor;
    }
    return "";
}