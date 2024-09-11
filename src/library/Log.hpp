#ifndef LOG_HPP
#define LOG_HPP

#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <string>

class Log
{
public:
    // Logs content to the console
    void logToConsole(const std::string &content);

private:
    // Get current formatted time
    std::string getCurrentTime();
};

#endif // LOG_HPP
