#include "syntaxanalyzer.h"
#include <sstream>

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errorMgr)
    : tokens(tokens), pos(0), errorMgr(errorMgr) {}

Token SyntaxAnalyzer::current() const {
    if (pos < tokens.size()) return tokens[pos];
    return Token(TokenType::EOF_TOKEN, "EOF", -1, -1);
}

Token SyntaxAnalyzer::advance() {
    Token t = current();
    if (pos < tokens.size()) pos++;
    return t;
}

bool SyntaxAnalyzer::check(TokenType type) const {
    return current().type == type;
}

bool SyntaxAnalyzer::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void SyntaxAnalyzer::synchronize() {
    advance();
    while (current().type != TokenType::EOF_TOKEN) {
        if (current().type == TokenType::PUNTO_COMA ||
            current().type == TokenType::LLAVE_C ||
            current().type == TokenType::CORCHETE_C) {
            return;
        }
        advance();
    }
}

bool SyntaxAnalyzer::parse() {
    root = programa();
    return !errorMgr.hasErrors();
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::programa() {
    auto node = std::make_shared<ASTNode>("<programa>");

    if (!match(TokenType::TABLERO)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba 'TABLERO' al inicio del archivo.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("TABLERO", true, "TABLERO"));

    if (!check(TokenType::CADENA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba nombre del tablero (CADENA) después de 'TABLERO'.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    auto tok = advance();
    node->addChild(std::make_shared<ASTNode>("CADENA", true, tok.lexeme));
    tablero.nombre = tok.lexeme;

    if (!match(TokenType::LLAVE_A)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba '{' después del nombre del tablero.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("{", true, "{"));

    auto cols = columnas();
    node->addChild(cols);

    if (!match(TokenType::LLAVE_C)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba '}' al cerrar el tablero.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>("}", true, "}"));
    }

    if (!match(TokenType::PUNTO_COMA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba ';' al final del tablero.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>(";", true, ";"));
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::columnas() {
    auto node = std::make_shared<ASTNode>("<columnas>");

    if (check(TokenType::COLUMNA)) {
        auto col = columna();
        node->addChild(col);

        if (check(TokenType::COLUMNA)) {
            auto rest = columnas();
            for (auto& child : rest->children) {
                node->addChild(child);
            }
        }
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::columna() {
    auto node = std::make_shared<ASTNode>("<columna>");
    Columna colData;

    if (!match(TokenType::COLUMNA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba 'COLUMNA'.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("COLUMNA", true, "COLUMNA"));

    if (!check(TokenType::CADENA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba nombre de columna (CADENA).",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    auto tok = advance();
    node->addChild(std::make_shared<ASTNode>("CADENA", true, tok.lexeme));
    colData.nombre = tok.lexeme;

    if (!match(TokenType::LLAVE_A)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba '{' después del nombre de columna.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("{", true, "{"));

    auto tars = tareas();
    node->addChild(tars);

    if (!match(TokenType::LLAVE_C)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba '}' al cerrar la columna.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>("}", true, "}"));
    }

    if (!match(TokenType::PUNTO_COMA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba ';' al final de la columna.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>(";", true, ";"));
    }

    // Extraer tareas del nodo para el modelo de datos
    for (auto& child : tars->children) {
        // Cada child de tareas es <tarea>, que contiene los datos
        // Simplificación: reconstruiremos desde tokens
    }
    tablero.columnas.push_back(colData);

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::tareas() {
    auto node = std::make_shared<ASTNode>("<tareas>");

    if (check(TokenType::TAREA)) {
        auto t = tarea();
        node->addChild(t);

        if (check(TokenType::COMA)) {
            auto comma = advance();
            node->addChild(std::make_shared<ASTNode>(",", true, ","));

            auto rest = tareas();
            for (auto& child : rest->children) {
                node->addChild(child);
            }
        }
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::tarea() {
    auto node = std::make_shared<ASTNode>("<tarea>");
    Tarea tareaData;

    if (!match(TokenType::TAREA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba 'tarea'.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("tarea", true, "tarea"));

    if (!match(TokenType::DOS_PUNTOS)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba ':' después de 'tarea'.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>(":", true, ":"));
    }

    if (!check(TokenType::CADENA)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba nombre de tarea (CADENA).",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    auto tok = advance();
    node->addChild(std::make_shared<ASTNode>("CADENA", true, tok.lexeme));
    tareaData.nombre = tok.lexeme;

    if (!match(TokenType::CORCHETE_A)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba '[' antes de los atributos.",
                          current().line, current().column, "ERROR");
        synchronize();
        return node;
    }
    node->addChild(std::make_shared<ASTNode>("[", true, "["));

    auto attrs = atributos();
    node->addChild(attrs);

    if (!match(TokenType::CORCHETE_C)) {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba ']' después de los atributos.",
                          current().line, current().column, "ERROR");
    } else {
        node->addChild(std::make_shared<ASTNode>("]", true, "]"));
    }

    // Guardar tarea en la columna actual (necesitamos referencia)
    // Nota: En implementación real, pasar referencia de columna actual
    if (!tablero.columnas.empty()) {
        tablero.columnas.back().tareas.push_back(tareaData);
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::atributos() {
    auto node = std::make_shared<ASTNode>("<atributos>");

    if (check(TokenType::PRIORIDAD) || check(TokenType::RESPONSABLE) ||
        check(TokenType::FECHA_LIMITE)) {
        auto attr = atributo();
        node->addChild(attr);

        if (check(TokenType::COMA)) {
            auto comma = advance();
            node->addChild(std::make_shared<ASTNode>(",", true, ","));

            auto rest = atributos();
            for (auto& child : rest->children) {
                node->addChild(child);
            }
        }
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::atributo() {
    auto node = std::make_shared<ASTNode>("<atributo>");
    Atributo attrData;

    if (check(TokenType::PRIORIDAD)) {
        advance();
        node->addChild(std::make_shared<ASTNode>("prioridad", true, "prioridad"));
        attrData.tipo = "prioridad";

        if (!match(TokenType::DOS_PUNTOS)) {
            errorMgr.addError(current().lexeme, "Sintáctico",
                              "Se esperaba ':' después de 'prioridad'.",
                              current().line, current().column, "ERROR");
        } else {
            node->addChild(std::make_shared<ASTNode>(":", true, ":"));
        }

        auto p = prioridad_valor();
        node->addChild(p);
        attrData.valor = p->value;
    }
    else if (check(TokenType::RESPONSABLE)) {
        advance();
        node->addChild(std::make_shared<ASTNode>("responsable", true, "responsable"));
        attrData.tipo = "responsable";

        if (!match(TokenType::DOS_PUNTOS)) {
            errorMgr.addError(current().lexeme, "Sintáctico",
                              "Se esperaba ':' después de 'responsable'.",
                              current().line, current().column, "ERROR");
        } else {
            node->addChild(std::make_shared<ASTNode>(":", true, ":"));
        }

        if (!check(TokenType::CADENA)) {
            errorMgr.addError(current().lexeme, "Sintáctico",
                              "Se esperaba nombre de responsable (CADENA).",
                              current().line, current().column, "ERROR");
        } else {
            auto tok = advance();
            node->addChild(std::make_shared<ASTNode>("CADENA", true, tok.lexeme));
            attrData.valor = tok.lexeme;
        }
    }
    else if (check(TokenType::FECHA_LIMITE)) {
        advance();
        node->addChild(std::make_shared<ASTNode>("fecha_limite", true, "fecha_limite"));
        attrData.tipo = "fecha_limite";

        if (!match(TokenType::DOS_PUNTOS)) {
            errorMgr.addError(current().lexeme, "Sintáctico",
                              "Se esperaba ':' después de 'fecha_limite'.",
                              current().line, current().column, "ERROR");
        } else {
            node->addChild(std::make_shared<ASTNode>(":", true, ":"));
        }

        if (!check(TokenType::FECHA)) {
            errorMgr.addError(current().lexeme, "Sintáctico",
                              "Se esperaba fecha en formato AAAA-MM-DD.",
                              current().line, current().column, "ERROR");
        } else {
            auto tok = advance();
            node->addChild(std::make_shared<ASTNode>("FECHA", true, tok.lexeme));
            attrData.valor = tok.lexeme;
        }
    }
    else {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba atributo (prioridad, responsable o fecha_limite).",
                          current().line, current().column, "ERROR");
        synchronize();
    }

    // Agregar atributo a la última tarea de la última columna
    if (!tablero.columnas.empty() && !tablero.columnas.back().tareas.empty()) {
        tablero.columnas.back().tareas.back().atributos.push_back(attrData);
    }

    return node;
}

std::shared_ptr<ASTNode> SyntaxAnalyzer::prioridad_valor() {
    auto node = std::make_shared<ASTNode>("<prioridad_valor>");

    if (check(TokenType::ALTA) || check(TokenType::MEDIA) || check(TokenType::BAJA)) {
        auto tok = advance();
        node->addChild(std::make_shared<ASTNode>(tok.lexeme, true, tok.lexeme));
        node->value = tok.lexeme;
    } else {
        errorMgr.addError(current().lexeme, "Sintáctico",
                          "Se esperaba ALTA, MEDIA o BAJA.",
                          current().line, current().column, "ERROR");
    }

    return node;
}