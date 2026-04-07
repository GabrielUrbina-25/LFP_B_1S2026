#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>

enum class TokenType {
    // Palabras reservadas raíz y secciones
    HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS,
    // Elementos
    PACIENTE, MEDICO, CITA, DIAGNOSTICO, CON,
    // Campos/atributos
    EDAD, TIPO_SANGRE, HABITACION, ESPECIALIDAD, CODIGO,
    FECHA, HORA, CONDICION, MEDICAMENTO, DOSIS,
    // Enumeraciones especialidad
    CARDIOLOGIA, NEUROLOGIA, PEDIATRIA, CIRUGIA,
    MEDICINA_GENERAL, ONCOLOGIA,
    // Enumeraciones dosis
    DIARIA, CADA_8_HORAS, CADA_12_HORAS, SEMANAL,
    // Literales
    CODE_ID,
    DATE_LITERAL,
    TIME_LITERAL,
    INTEGER,
    STRING,
    BLOOD_TYPE,
    // Delimitadores
    LBRACE, RBRACE, LBRACKET, RBRACKET, COLON, COMMA, SEMICOLON,
    // Especial
    END_OF_FILE,
    UNKNOWN
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t = TokenType::UNKNOWN, const std::string& lex = "", int ln = 0, int col = 0);

    static std::string typeToString(TokenType type);
    static std::map<TokenType, std::string> typeNames;
};

#endif