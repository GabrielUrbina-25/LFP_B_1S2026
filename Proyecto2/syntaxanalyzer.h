#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <vector>
#include <memory>
#include "token.h"
#include "errormanager.h"
#include "kanbandata.h"

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errorMgr);
    bool parse();
    std::shared_ptr<ASTNode> getRoot() const { return root; }
    Tablero getTablero() const { return tablero; }

private:
    std::vector<Token> tokens;
    size_t pos;
    ErrorManager& errorMgr;
    std::shared_ptr<ASTNode> root;
    Tablero tablero;

    Token current() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    void synchronize();

    // Funciones por producción de la GLC
    std::shared_ptr<ASTNode> programa();
    std::shared_ptr<ASTNode> columnas();
    std::shared_ptr<ASTNode> columna();
    std::shared_ptr<ASTNode> tareas();
    std::shared_ptr<ASTNode> tarea();
    std::shared_ptr<ASTNode> atributos();
    std::shared_ptr<ASTNode> atributo();
    std::shared_ptr<ASTNode> prioridad_valor();
};

#endif