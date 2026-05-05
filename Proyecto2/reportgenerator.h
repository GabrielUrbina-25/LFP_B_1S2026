#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include "kanbandata.h"
#include "token.h"
#include "errormanager.h"

class ReportGenerator {
public:
    ReportGenerator(const Tablero& tablero, const std::vector<Token>& tokens,
                    const ErrorManager& errorMgr, const std::shared_ptr<ASTNode>& root);

    void generateKanbanReport(const std::string& filename);      // Reporte 1
    void generateResponsableReport(const std::string& filename);  // Reporte 2
    void generateColumnasReport(const std::string& filename);     // Reporte 3
    void generateErrorsReport(const std::string& filename);       // Tabla tokens + errores
    void generateDOT(const std::string& filename);

private:
    Tablero tablero;
    std::vector<Token> tokens;
    ErrorManager errorMgr;
    std::shared_ptr<ASTNode> root;
    int dotCounter;

    std::string getPriorityColor(const std::string& p) const;
    void generateDOTNode(std::ofstream& out, std::shared_ptr<ASTNode> node, int& counter);
};

#endif