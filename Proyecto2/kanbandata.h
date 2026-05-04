#ifndef KANBANDATA_H
#define KANBANDATA_H

#include <string>
#include <vector>
#include <memory>

struct Atributo {
    std::string tipo;      // "prioridad", "responsable", "fecha_limite"
    std::string valor;
};

struct Tarea {
    std::string nombre;
    std::vector<Atributo> atributos;

    std::string getPrioridad() const;
    std::string getResponsable() const;
    std::string getFechaLimite() const;
};

struct Columna {
    std::string nombre;
    std::vector<Tarea> tareas;
};

struct Tablero {
    std::string nombre;
    std::vector<Columna> columnas;
};

// Nodo para el árbol de derivación
struct ASTNode {
    std::string label;
    std::string value; // Para terminales
    bool isTerminal;
    std::vector<std::shared_ptr<ASTNode>> children;

    ASTNode(const std::string& l, bool terminal = false, const std::string& v = "")
        : label(l), value(v), isTerminal(terminal) {}

    void addChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
};

#endif