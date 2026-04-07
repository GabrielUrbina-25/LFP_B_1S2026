#include "Token.h"

std::map<TokenType, std::string> Token::typeNames = {
    {TokenType::HOSPITAL, "HOSPITAL"}, {TokenType::PACIENTES, "PACIENTES"},
    {TokenType::MEDICOS, "MEDICOS"}, {TokenType::CITAS, "CITAS"},
    {TokenType::DIAGNOSTICOS, "DIAGNOSTICOS"}, {TokenType::PACIENTE, "PACIENTE"},
    {TokenType::MEDICO, "MEDICO"}, {TokenType::CITA, "CITA"},
    {TokenType::DIAGNOSTICO, "DIAGNOSTICO"}, {TokenType::CON, "CON"},
    // Campos/atributos
    {TokenType::EDAD, "EDAD"}, {TokenType::TIPO_SANGRE, "TIPO_SANGRE"},
    {TokenType::HABITACION, "HABITACION"}, {TokenType::ESPECIALIDAD, "ESPECIALIDAD"},
    {TokenType::CODIGO, "CODIGO"}, {TokenType::FECHA, "FECHA"},
    {TokenType::HORA, "HORA"}, {TokenType::CONDICION, "CONDICION"},
    {TokenType::MEDICAMENTO, "MEDICAMENTO"}, {TokenType::DOSIS, "DOSIS"},
    // Enumeraciones
    {TokenType::CARDIOLOGIA, "CARDIOLOGIA"}, {TokenType::NEUROLOGIA, "NEUROLOGIA"},
    {TokenType::PEDIATRIA, "PEDIATRIA"}, {TokenType::CIRUGIA, "CIRUGIA"},
    {TokenType::MEDICINA_GENERAL, "MEDICINA_GENERAL"}, {TokenType::ONCOLOGIA, "ONCOLOGIA"},
    {TokenType::DIARIA, "DIARIA"}, {TokenType::CADA_8_HORAS, "CADA_8_HORAS"},
    {TokenType::CADA_12_HORAS, "CADA_12_HORAS"}, {TokenType::SEMANAL, "SEMANAL"},
    // Literales y delimitadores
    {TokenType::CODE_ID, "CODE_ID"}, {TokenType::DATE_LITERAL, "DATE_LITERAL"},
    {TokenType::TIME_LITERAL, "TIME_LITERAL"}, {TokenType::INTEGER, "INTEGER"},
    {TokenType::STRING, "STRING"}, {TokenType::BLOOD_TYPE, "BLOOD_TYPE"},
    {TokenType::LBRACE, "LBRACE"}, {TokenType::RBRACE, "RBRACE"},
    {TokenType::LBRACKET, "LBRACKET"}, {TokenType::RBRACKET, "RBRACKET"},
    {TokenType::COLON, "COLON"}, {TokenType::COMMA, "COMMA"},
    {TokenType::SEMICOLON, "SEMICOLON"}, {TokenType::END_OF_FILE, "EOF"},
    {TokenType::UNKNOWN, "UNKNOWN"}
};

Token::Token(TokenType t, const std::string& lex, int ln, int col)
    : type(t), lexeme(lex), line(ln), column(col) {}

std::string Token::typeToString(TokenType type) {
    auto it = typeNames.find(type);
    if (it != typeNames.end()) return it->second;
    return "UNKNOWN";
}