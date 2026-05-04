#include "errormanager.h"

void ErrorManager::addError(const std::string& lexeme, const std::string& type,
                            const std::string& desc, int line, int col,
                            const std::string& severity) {
    Error err;
    err.number = counter++;
    err.lexeme = lexeme;
    err.type = type;
    err.description = desc;
    err.line = line;
    err.column = col;
    err.severity = severity;
    errors.push_back(err);
}