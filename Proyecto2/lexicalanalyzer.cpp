#include "lexicalanalyzer.h"
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(const std::string& input, ErrorManager& errorMgr)
    : input(input), pos(0), line(1), column(1), errorMgr(errorMgr), finished(false) {}

char LexicalAnalyzer::peek() const {
    if (pos >= input.size()) return '\0';
    return input[pos];
}

char LexicalAnalyzer::advance() {
    char c = peek();
    if (c == '\n') { line++; column = 1; }
    else { column++; }
    pos++;
    return c;
}

void LexicalAnalyzer::skipWhitespace() {
    while (isspace(peek())) advance();
}

Token LexicalAnalyzer::makeToken(TokenType type, const std::string& lexeme, int l, int c) {
    return Token(type, lexeme, l, c);
}

bool LexicalAnalyzer::isDelimiter(char c) const {
    return c == '{' || c == '}' || c == '[' || c == ']' ||
           c == ':' || c == ',' || c == ';';
}

Token LexicalAnalyzer::nextToken() {
    if (finished) return Token(TokenType::EOF_TOKEN, "EOF", line, column);

    skipWhitespace();

    if (pos >= input.size()) {
        finished = true;
        return Token(TokenType::EOF_TOKEN, "EOF", line, column);
    }

    int startLine = line;
    int startCol = column;
    char c = peek();

    // Estado inicial q0 - reconocimiento de delimitadores (q5)
    if (isDelimiter(c)) {
        advance();
        switch(c) {
        case '{': return makeToken(TokenType::LLAVE_A, "{", startLine, startCol);
        case '}': return makeToken(TokenType::LLAVE_C, "}", startLine, startCol);
        case '[': return makeToken(TokenType::CORCHETE_A, "[", startLine, startCol);
        case ']': return makeToken(TokenType::CORCHETE_C, "]", startLine, startCol);
        case ':': return makeToken(TokenType::DOS_PUNTOS, ":", startLine, startCol);
        case ',': return makeToken(TokenType::COMA, ",", startLine, startCol);
        case ';': return makeToken(TokenType::PUNTO_COMA, ";", startLine, startCol);
        }
    }

    // q0 + letra → q1 (identificador/palabra reservada)
    if (isalpha(c) || c == '_') {
        std::string lexeme;
        while (isalnum(peek()) || peek() == '_') {
            lexeme += advance();
        }

        // Verificar palabras reservadas y enumeraciones
        if (lexeme == "TABLERO") return makeToken(TokenType::TABLERO, lexeme, startLine, startCol);
        if (lexeme == "COLUMNA") return makeToken(TokenType::COLUMNA, lexeme, startLine, startCol);
        if (lexeme == "tarea") return makeToken(TokenType::TAREA, lexeme, startLine, startCol);
        if (lexeme == "prioridad") return makeToken(TokenType::PRIORIDAD, lexeme, startLine, startCol);
        if (lexeme == "responsable") return makeToken(TokenType::RESPONSABLE, lexeme, startLine, startCol);
        if (lexeme == "fecha_limite") return makeToken(TokenType::FECHA_LIMITE, lexeme, startLine, startCol);
        if (lexeme == "ALTA") return makeToken(TokenType::ALTA, lexeme, startLine, startCol);
        if (lexeme == "MEDIA") return makeToken(TokenType::MEDIA, lexeme, startLine, startCol);
        if (lexeme == "BAJA") return makeToken(TokenType::BAJA, lexeme, startLine, startCol);

        // Si no es reservada, es un identificador no válido en este lenguaje
        errorMgr.addError(lexeme, "Léxico",
                          "Identificador no reconocido. Se esperaba palabra reservada.",
                          startLine, startCol, "ERROR");
        return makeToken(TokenType::UNKNOWN, lexeme, startLine, startCol);
    }

    // q0 + " → q2 (cadena)
    if (c == '"') {
        std::string lexeme;
        lexeme += advance(); // consume comilla inicial

        while (peek() != '"' && peek() != '\0' && peek() != '\n') {
            lexeme += advance();
        }

        if (peek() == '"') {
            lexeme += advance(); // consume comilla final
            return makeToken(TokenType::CADENA, lexeme, startLine, startCol);
        } else {
            // Cadena no cerrada - error crítico pero seguimos
            errorMgr.addError(lexeme, "Léxico",
                              "Cadena no cerrada antes del fin de línea.",
                              startLine, startCol, "CRITICO");
            return makeToken(TokenType::UNKNOWN, lexeme, startLine, startCol);
        }
    }

    // q0 + dígito → q3 (número/fecha)
    if (isdigit(c)) {
        std::string lexeme;
        while (isdigit(peek())) {
            lexeme += advance();
        }

        // Verificar si es fecha (patrón AAAA-MM-DD)
        if (peek() == '-' && lexeme.size() == 4) {
            // Posible fecha: ya tenemos AAAA
            lexeme += advance(); // consume '-'

            // Esperar MM
            int digits = 0;
            while (isdigit(peek()) && digits < 2) {
                lexeme += advance();
                digits++;
            }

            if (digits == 2 && peek() == '-') {
                lexeme += advance(); // consume '-'

                // Esperar DD
                digits = 0;
                while (isdigit(peek()) && digits < 2) {
                    lexeme += advance();
                    digits++;
                }

                if (digits == 2) {
                    // Validar rango básico
                    return makeToken(TokenType::FECHA, lexeme, startLine, startCol);
                }
            }

            // Si llegamos aquí, el formato de fecha es incorrecto
            errorMgr.addError(lexeme, "Léxico",
                              "Formato de fecha inválido. Se esperaba AAAA-MM-DD.",
                              startLine, startCol, "ERROR");
            return makeToken(TokenType::UNKNOWN, lexeme, startLine, startCol);
        }

        return makeToken(TokenType::ENTERO, lexeme, startLine, startCol);
    }

    // q6 - Error léxico (carácter no reconocido)
    std::string bad(1, advance());
    errorMgr.addError(bad, "Léxico",
                      "Carácter no reconocido '" + bad + "'.",
                      startLine, startCol, "ERROR");
    return makeToken(TokenType::UNKNOWN, bad, startLine, startCol);
}

const std::vector<Token>& LexicalAnalyzer::getAllTokens() {
    if (!tokens.empty()) return tokens;

    Token tok = nextToken();
    while (tok.type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
        tok = nextToken();
    }
    tokens.push_back(tok); // EOF
    return tokens;
}