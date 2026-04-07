#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include "Token.h"
#include "ErrorManager.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>

struct AnalysisStats {
    std::map<TokenType, int> tokenFrequency;
    double analysisTimeMs = 0.0;
    int totalTokens = 0;
};

class LexicalAnalyzer {
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    ErrorManager& errorManager;
    AnalysisStats stats;
    bool collectingStats;
    std::chrono::high_resolution_clock::time_point startTime;

    enum class State {
        S0, S_ID, S_NUMBER, S_DATE_MONTH, S_DATE_DAY, S_TIME_MIN,
        S_STRING, S_ERROR, S_ACCEPT
    };

    char currentChar();
    void advance();
    void skipWhitespace();
    bool isLetter(char c);
    bool isDigit(char c);
    bool isAlphaNum(char c);
    TokenType checkReservedWord(const std::string& word);
    bool validateDate(const std::string& date);
    bool validateTime(const std::string& time);
    bool isBloodType(const std::string& str);

public:
    LexicalAnalyzer(const std::string& src, ErrorManager& em);
    Token nextToken();
    const AnalysisStats& getStats() const;
    void startStatsCollection();
    void endStatsCollection();
};

#endif