#ifndef ERRORMANAGER_H
#define ERRORMANAGER_H

#include <vector>
#include <string>

struct Error {
    int number;
    std::string lexeme;
    std::string type;      // "Léxico" o "Sintáctico"
    std::string description;
    int line;
    int column;
    std::string severity;  // "ERROR" o "CRITICO"
};

class ErrorManager {
public:
    void addError(const std::string& lexeme, const std::string& type,
                  const std::string& desc, int line, int col,
                  const std::string& severity = "ERROR");
    const std::vector<Error>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clear() { errors.clear(); counter = 1; }

private:
    std::vector<Error> errors;
    int counter = 1;
};

#endif