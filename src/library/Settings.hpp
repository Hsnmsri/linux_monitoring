#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>  // Include the JSON library

using json = nlohmann::json;

class Settings {
public:
    // Constructor
    Settings() = default;

    // Load settings from the JSON file
    bool getSetting();

    // Getter functions to access private member variables
    std::string getBotToken() const { return botToken; }
    int64_t getChatId() const { return chatId; }
    int getCpuCheckDuration() const { return cpuCheckDuration; }
    int getMemoryCheckDuration() const { return memoryCheckDuration; }
    int getCpuLimit() const { return cpuLimit; }
    int getMemoryLimit() const { return memoryLimit; }

private:
    std::string botToken;
    int64_t chatId;
    int cpuCheckDuration;
    int memoryCheckDuration;
    int cpuLimit;
    int memoryLimit;
};

#endif // SETTINGS_HPP
