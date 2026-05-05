#include "token.h"

std::string Token::typeToString(TokenType type) {
    switch(type) {
    case TokenType::TABLERO: return "TABLERO";
    case TokenType::COLUMNA: return "COLUMNA";
    case TokenType::TAREA: return "TAREA";
    case TokenType::PRIORIDAD: return "PRIORIDAD";
    case TokenType::RESPONSABLE: return "RESPONSABLE";
    case TokenType::FECHA_LIMITE: return "FECHA_LIMITE";
    case TokenType::ALTA: return "ALTA";
    case TokenType::MEDIA: return "MEDIA";
    case TokenType::BAJA: return "BAJA";
    case TokenType::CADENA: return "CADENA";
    case TokenType::FECHA: return "FECHA";
    case TokenType::ENTERO: return "ENTERO";
    case TokenType::LLAVE_A: return "LLAVE_A";
    case TokenType::LLAVE_C: return "LLAVE_C";
    case TokenType::CORCHETE_A: return "CORCHETE_A";
    case TokenType::CORCHETE_C: return "CORCHETE_C";
    case TokenType::DOS_PUNTOS: return "DOS_PUNTOS";
    case TokenType::COMA: return "COMA";
    case TokenType::PUNTO_COMA: return "PUNTO_COMA";
    case TokenType::EOF_TOKEN: return "EOF";
    case TokenType::UNKNOWN: return "UNKNOWN";
    }
    return "UNKNOWN";
}