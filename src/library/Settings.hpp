#pragma once

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp> // Include the JSON library

using json = nlohmann::json;

class Settings
{
public:
    // Constructor
    Settings() = default;

    // Load settings from the JSON file
    bool getSetting();

    // Getter functions to access private member variables
    std::string getBotToken() const { return botToken; }
    int64_t getChatId() const { return chatId; }
    std::string getAppVersion() const { return app_version; }
    std::string getServerName() const { return server_name; }
    int getCpuCheckDuration() const { return cpuCheckDuration; }
    int getMemoryCheckDuration() const { return memoryCheckDuration; }
    int getCpuLimit() const { return cpuLimit; }
    int getMemoryLimit() const { return memoryLimit; }

private:
    std::string botToken;
    int64_t chatId;
    std::string app_version;
    std::string server_name;
    int cpuCheckDuration;
    int memoryCheckDuration;
    int cpuLimit;
    int memoryLimit;
};
