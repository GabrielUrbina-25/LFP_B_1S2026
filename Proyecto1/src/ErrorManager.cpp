#include "ErrorManager.h"

ErrorManager::ErrorManager() : errorCounter(0) {}

void ErrorManager::addError(const std::string& lexeme, const std::string& type,
                            const std::string& description, int line, int column,
                            ErrorSeverity severity) {
    LexicalError err;
    err.number = ++errorCounter;
    err.lexeme = lexeme;
    err.type = type;
    err.description = description;
    err.line = line;
    err.column = column;
    err.severity = severity;
    errors.push_back(err);
}

const std::vector<LexicalError>& ErrorManager::getErrors() const {
    return errors;
}

bool ErrorManager::hasErrors() const {
    return !errors.empty();
}

void ErrorManager::clear() {
    errors.clear();
    errorCounter = 0;
}

std::string ErrorManager::generateErrorReport() const {
    std::stringstream ss;
    ss << "<html><head><style>"
       << "body{font-family:Arial,sans-serif;margin:20px;}"
       << "table{border-collapse:collapse;width:100%;}"
       << "th{background:#e74c3c;color:white;padding:10px;}"
       << "td{border:1px solid #ddd;padding:8px;}"
       << ".critico{color:#e74c3c;font-weight:bold;}"
       << "</style></head><body><h1>Tabla de Errores Lexicos</h1>"
       << "<table><tr><th>No.</th><th>Lexema</th><th>Tipo</th>"
       << "<th>Descripcion</th><th>Linea</th><th>Columna</th><th>Gravedad</th></tr>";

    for (const auto& err : errors) {
        std::string gravedad = (err.severity == ErrorSeverity::CRITICAL) ? "CRITICO" : "ERROR";
        std::string clase = (err.severity == ErrorSeverity::CRITICAL) ? "class='critico'" : "";
        ss << "<tr " << clase << "><td>" << err.number << "</td><td>" << err.lexeme
           << "</td><td>" << err.type << "</td><td>" << err.description
           << "</td><td>" << err.line << "</td><td>" << err.column
           << "</td><td>" << gravedad << "</td></tr>";
    }
    ss << "</table></body></html>";
    return ss.str();
}