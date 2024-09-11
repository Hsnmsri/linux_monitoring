#include "Log.hpp"

// Method to log content to the console
void Log::logToConsole(const std::string& content) {
    std::cout << "[" << getCurrentTime() << "] " << content << std::endl;
}

// Private method to get the current time as a formatted string
std::string Log::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm *localTime = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
