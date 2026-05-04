#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <string>
#include <vector>
#include "token.h"
#include "errormanager.h"

class LexicalAnalyzer {
public:
    LexicalAnalyzer(const std::string& input, ErrorManager& errorMgr);
    Token nextToken();
    const std::vector<Token>& getAllTokens();

private:
    std::string input;
    size_t pos;
    int line;
    int column;
    ErrorManager& errorMgr;
    std::vector<Token> tokens;
    bool finished;

    char peek() const;
    char advance();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& lexeme, int l, int c);
    bool isDelimiter(char c) const;
};

#endif