#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <chrono>

enum class LogLevel
{
  DEBUG,
  INFO,
  ERROR,
  SUCCESS,
  WARNING,
};

std::string levelToString(LogLevel level);
std::string levelToColor(LogLevel level);

struct LogMessage
{
  LogLevel level;
  std::string message;
  std::chrono::system_clock::time_point timestamp;

  LogMessage(LogLevel lvl, const std::string& msg);
};

class Logger {
  public:
    static Logger& getInstance();
    void setMinLogLevel(LogLevel level);
    void log(const LogMessage& msg);

    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;

  private:
    Logger();
    ~Logger();
    std::ofstream file;
    LogLevel minLevel;
    std::mutex logMutex;

    std::string getTimeString(const std::chrono::system_clock::time_point& time);
};

#define LOG(level, msg) Logger::getInstance().log(LogMessage(level, msg))
