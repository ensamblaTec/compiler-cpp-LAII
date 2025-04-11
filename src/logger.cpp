#include "logger.hpp"

#include <iostream>
#include <ctime>

std::string levelToString(LogLevel level)
{
  switch (level)
  {
    case LogLevel::DEBUG: return "DEBUG";
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::SUCCESS: return "SUCCESS";
    default: return "UNKNOWN";
  }
}

std::string levelToColor(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "\033[36m";    // Cyan
        case LogLevel::INFO: return "\033[32m";     // Green
        case LogLevel::WARNING: return "\033[33m";  // Yellow
        case LogLevel::ERROR: return "\033[31m";    // Red
        default: return "\033[0m";                  // Reset
    }
}

LogMessage::LogMessage(LogLevel lvl, const std::string& msg)
  : level(lvl), message(msg), timestamp(std::chrono::system_clock::now()) {}

Logger::Logger() : minLevel(LogLevel::DEBUG) 
{
  file.open("log.txt", std::ios::app);
}

Logger::~Logger()
{
  if (file.is_open()) file.close();
}

Logger& Logger::getInstance()
{
  static Logger instance;
  return instance;
}

void Logger::setMinLogLevel(LogLevel level)
{
  minLevel = level;
}

std::string Logger::getTimeString(const std::chrono::system_clock::time_point& time)
{
  auto timeT = std::chrono::system_clock::to_time_t(time);
  char buffer[100];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&timeT));
  return std::string(buffer);
}

void Logger::log(const LogMessage& msg)
{
  if (msg.level < minLevel) return;

  std::lock_guard<std::mutex> lock(logMutex);
  std::string timeString = getTimeString(msg.timestamp);
  std::string levelString = levelToString(msg.level);
  std::string color = levelToColor(msg.level);

  std::cout << color
    << "[" << timeString << "] "
    << "[" << levelString << "] "
    << msg.message << std::endl;

  if (file.is_open()) 
  {
    file << "[" << timeString << "] "
      << "[" << levelString << "] "
      << msg.message << std::endl;
  }
}

