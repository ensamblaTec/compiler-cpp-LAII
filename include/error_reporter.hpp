#pragma once

#include <string>
#include <vector>

struct ErrorInfo {
    std::string message;
    int line;
    int column;
};

class ErrorReporter {
public:
    static ErrorReporter& getInstance();

    void report(const std::string& message, int line, int column);
    void printSummary() const;
    void clear();
    bool hasErrors() const;
    ErrorReporter() = default;
    std::vector<ErrorInfo> errors;
};

