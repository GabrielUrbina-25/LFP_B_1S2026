#include "LexicalAnalyzer.h"
#include <cctype>
#include <algorithm>

LexicalAnalyzer::LexicalAnalyzer(const std::string& src, ErrorManager& em)
    : source(src), pos(0), line(1), column(1), errorManager(em),
    collectingStats(false) {}

char LexicalAnalyzer::currentChar() {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

void LexicalAnalyzer::advance() {
    if (pos < source.length()) {
        if (source[pos] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

void LexicalAnalyzer::skipWhitespace() {
    while (std::isspace(static_cast<unsigned char>(currentChar()))) {
        advance();
    }
}

bool LexicalAnalyzer::isLetter(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || c == 'ñ' || c == 'Ñ';
}

bool LexicalAnalyzer::isDigit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool LexicalAnalyzer::isAlphaNum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == 'ñ' || c == 'Ñ';
}

TokenType LexicalAnalyzer::checkReservedWord(const std::string& word) {
    static const std::map<std::string, TokenType> reserved = {
        // Palabras reservadas principales
        {"HOSPITAL", TokenType::HOSPITAL},
        {"PACIENTES", TokenType::PACIENTES},
        {"MEDICOS", TokenType::MEDICOS},
        {"CITAS", TokenType::CITAS},
        {"DIAGNOSTICOS", TokenType::DIAGNOSTICOS},

        // Elementos (minúsculas)
        {"paciente", TokenType::PACIENTE},
        {"medico", TokenType::MEDICO},
        {"cita", TokenType::CITA},
        {"diagnostico", TokenType::DIAGNOSTICO},
        {"con", TokenType::CON},

        // Campos/atributos (minúsculas con guion bajo)
        {"edad", TokenType::EDAD},
        {"tipo_sangre", TokenType::TIPO_SANGRE},
        {"habitacion", TokenType::HABITACION},
        {"especialidad", TokenType::ESPECIALIDAD},
        {"codigo", TokenType::CODIGO},
        {"fecha", TokenType::FECHA},
        {"hora", TokenType::HORA},
        {"condicion", TokenType::CONDICION},
        {"medicamento", TokenType::MEDICAMENTO},
        {"dosis", TokenType::DOSIS},

        // Enumeraciones especialidad (mayúsculas)
        {"CARDIOLOGIA", TokenType::CARDIOLOGIA},
        {"NEUROLOGIA", TokenType::NEUROLOGIA},
        {"PEDIATRIA", TokenType::PEDIATRIA},
        {"CIRUGIA", TokenType::CIRUGIA},
        {"MEDICINA_GENERAL", TokenType::MEDICINA_GENERAL},
        {"ONCOLOGIA", TokenType::ONCOLOGIA},

        // Enumeraciones dosis (mayúsculas con guion bajo)
        {"DIARIA", TokenType::DIARIA},
        {"CADA_8_HORAS", TokenType::CADA_8_HORAS},
        {"CADA_12_HORAS", TokenType::CADA_12_HORAS},
        {"SEMANAL", TokenType::SEMANAL}
    };

    auto it = reserved.find(word);
    if (it != reserved.end()) return it->second;
    return TokenType::UNKNOWN;
}

bool LexicalAnalyzer::validateDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    try {
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        if (month == 2 && day > 29) return false;
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
        return true;
    } catch (...) {
        return false;
    }
}

bool LexicalAnalyzer::validateTime(const std::string& time) {
    if (time.length() != 5) return false;
    if (time[2] != ':') return false;

    try {
        int hour = std::stoi(time.substr(0, 2));
        int min = std::stoi(time.substr(3, 2));
        return (hour >= 0 && hour <= 23 && min >= 0 && min <= 59);
    } catch (...) {
        return false;
    }
}

bool LexicalAnalyzer::isBloodType(const std::string& str) {
    return (str == "A+" || str == "A-" || str == "B+" || str == "B-" ||
            str == "O+" || str == "O-" || str == "AB+" || str == "AB-");
}

void LexicalAnalyzer::startStatsCollection() {
    collectingStats = true;
    stats.tokenFrequency.clear();
    stats.analysisTimeMs = 0;
    stats.totalTokens = 0;
    startTime = std::chrono::high_resolution_clock::now();
}

void LexicalAnalyzer::endStatsCollection() {
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.analysisTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
}

const AnalysisStats& LexicalAnalyzer::getStats() const {
    return stats;
}

Token LexicalAnalyzer::nextToken() {
    skipWhitespace();

    if (currentChar() == '\0') {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }

    int startLine = line;
    int startCol = column;
    std::string lexeme;
    State state = State::S0;

    // AFD - Automata Finito Determinista
    while (state != State::S_ACCEPT && state != State::S_ERROR) {
        char c = currentChar();

        switch (state) {
        case State::S0:
            // Delimitadores simples (un solo carácter)
            if (c == '{') { advance(); return Token(TokenType::LBRACE, "{", startLine, startCol); }
            if (c == '}') { advance(); return Token(TokenType::RBRACE, "}", startLine, startCol); }
            if (c == '[') { advance(); return Token(TokenType::LBRACKET, "[", startLine, startCol); }
            if (c == ']') { advance(); return Token(TokenType::RBRACKET, "]", startLine, startCol); }
            if (c == ':') { advance(); return Token(TokenType::COLON, ":", startLine, startCol); }
            if (c == ',') { advance(); return Token(TokenType::COMMA, ",", startLine, startCol); }
            if (c == ';') { advance(); return Token(TokenType::SEMICOLON, ";", startLine, startCol); }

            // Identificadores y palabras reservadas
            if (isLetter(c)) {
                lexeme += c;
                advance();
                state = State::S_ID;
            }
            // Números (enteros, fechas, horas)
            else if (isDigit(c)) {
                lexeme += c;
                advance();
                state = State::S_NUMBER;
            }
            // Strings entre comillas
            else if (c == '"') {
                lexeme += c;
                advance();
                state = State::S_STRING;
            }
            // Caracter inválido
            else {
                lexeme += c;
                advance();
                errorManager.addError(lexeme, "Caracter ilegal",
                                      "El caracter no es valido en MedLang", startLine, startCol);
                return Token(TokenType::UNKNOWN, lexeme, startLine, startCol);
            }
            break;

        case State::S_ID:
            // Continuar leyendo identificador (letras, números, guion bajo)
            if (isAlphaNum(c) || c == '_') {
                lexeme += c;
                advance();
            } else {
                state = State::S_ACCEPT;
            }
            break;

        case State::S_NUMBER:
            if (isDigit(c)) {
                lexeme += c;
                advance();
            } else if (c == '-' && lexeme.length() == 4) {
                // Posible fecha: 2025-
                lexeme += c;
                advance();
                state = State::S_DATE_MONTH;
            } else if (c == ':' && lexeme.length() == 2) {
                // Posible hora: 09:
                lexeme += c;
                advance();
                state = State::S_TIME_MIN;
            } else {
                state = State::S_ACCEPT;
            }
            break;

        case State::S_DATE_MONTH:
            if (isDigit(c) && lexeme.length() < 7) {
                lexeme += c;
                advance();
            } else if (c == '-' && lexeme.length() == 7) {
                // 2025-04-
                lexeme += c;
                advance();
                state = State::S_DATE_DAY;
            } else {
                state = State::S_ACCEPT;
            }
            break;

        case State::S_DATE_DAY:
            if (isDigit(c) && lexeme.length() < 10) {
                lexeme += c;
                advance();
            } else {
                state = State::S_ACCEPT;
            }
            break;

        case State::S_TIME_MIN:
            if (isDigit(c) && lexeme.length() < 5) {
                lexeme += c;
                advance();
            } else {
                state = State::S_ACCEPT;
            }
            break;

        case State::S_STRING:
            if (c == '\0' || c == '\n') {
                errorManager.addError(lexeme, "Cadena sin cerrar",
                                      "Inicio de cadena sin cierre antes de fin de linea",
                                      startLine, startCol, ErrorSeverity::CRITICAL);
                state = State::S_ERROR;
            } else if (c == '"') {
                lexeme += c;
                advance();
                state = State::S_ACCEPT;
            } else {
                // Aceptar cualquier carácter dentro del string (incluyendo tildes, espacios)
                lexeme += c;
                advance();
            }
            break;

        case State::S_ACCEPT:
        case State::S_ERROR:
            break;
        }
    }

    if (state == State::S_ERROR) {
        return Token(TokenType::UNKNOWN, lexeme, startLine, startCol);
    }

    // Determinar tipo de token
    TokenType type = TokenType::UNKNOWN;

    // String entre comillas
    if (lexeme.length() >= 2 && lexeme.front() == '"' && lexeme.back() == '"') {
        std::string content = lexeme.substr(1, lexeme.length() - 2);

        // Verificar si es tipo de sangre
        if (isBloodType(content)) {
            type = TokenType::BLOOD_TYPE;
        } else {
            type = TokenType::STRING;
        }
    }
    // Número entero
    else if (std::all_of(lexeme.begin(), lexeme.end(), ::isdigit)) {
        type = TokenType::INTEGER;
    }
    // Fecha: AAAA-MM-DD
    else if (lexeme.length() == 10 && lexeme[4] == '-' && lexeme[7] == '-') {
        if (validateDate(lexeme)) {
            type = TokenType::DATE_LITERAL;
        } else {
            errorManager.addError(lexeme, "Fecha invalida",
                                  "Formato AAAA-MM-DD con valores validos", startLine, startCol);
        }
    }
    // Hora: HH:MM
    else if (lexeme.length() == 5 && lexeme[2] == ':') {
        if (validateTime(lexeme)) {
            type = TokenType::TIME_LITERAL;
        } else {
            errorManager.addError(lexeme, "Hora fuera de rango",
                                  "Hora entre 00:00 y 23:59", startLine, startCol);
        }
    }
    // Código: MED-001 o PAC-001
    else {
        if (lexeme.length() > 4 && lexeme[3] == '-' &&
            (lexeme.substr(0,3) == "MED" || lexeme.substr(0,3) == "PAC")) {
            bool validCode = true;
            for (size_t i = 4; i < lexeme.length(); ++i) {
                if (!isDigit(lexeme[i])) {
                    validCode = false;
                    break;
                }
            }
            if (validCode) {
                type = TokenType::CODE_ID;
            }
        }

        // Palabra reservada o identificador
        if (type == TokenType::UNKNOWN) {
            type = checkReservedWord(lexeme);
        }
    }

    // Si sigue siendo UNKNOWN, es error léxico
    if (type == TokenType::UNKNOWN && !lexeme.empty()) {
        errorManager.addError(lexeme, "Token no reconocido",
                              "Lexema '" + lexeme + "' no valido en MedLang", startLine, startCol);
    }

    // Estadísticas
    if (collectingStats) {
        stats.tokenFrequency[type]++;
        stats.totalTokens++;
    }

    return Token(type, lexeme, startLine, startCol);
}