#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <vector>
#include <string>
#include <sstream>

enum class ErrorSeverity { ERROR, CRITICAL };

struct LexicalError {
    int number;
    std::string lexeme;
    std::string type;
    std::string description;
    int line;
    int column;
    ErrorSeverity severity;
};

class ErrorManager {
private:
    std::vector<LexicalError> errors;
    int errorCounter;

public:
    ErrorManager();
    void addError(const std::string& lexeme, const std::string& type,
                  const std::string& description, int line, int column,
                  ErrorSeverity severity = ErrorSeverity::ERROR);
    const std::vector<LexicalError>& getErrors() const;
    bool hasErrors() const;
    void clear();
    std::string generateErrorReport() const;
};

#endif