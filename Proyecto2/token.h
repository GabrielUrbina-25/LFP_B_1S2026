#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Palabras reservadas
    TABLERO, COLUMNA, TAREA, PRIORIDAD, RESPONSABLE, FECHA_LIMITE,
    // Enumeraciones
    ALTA, MEDIA, BAJA,
    // Literales
    CADENA, FECHA, ENTERO,
    // Delimitadores
    LLAVE_A, LLAVE_C, CORCHETE_A, CORCHETE_C, DOS_PUNTOS, COMA, PUNTO_COMA,
    // Especiales
    EOF_TOKEN, UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, std::string l, int ln, int col)
        : type(t), lexeme(std::move(l)), line(ln), column(col) {}

    static std::string typeToString(TokenType type);
};

#endif