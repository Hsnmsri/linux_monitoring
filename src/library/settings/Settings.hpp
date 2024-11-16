#pragma once

#include <iostream>
#include <fstream>
#include <node/NodeStructure.hpp>
#include <nlohmann/json.hpp> // Include the JSON library
#include <log/Log.hpp>

using json = nlohmann::json;

class Settings
{
public:
    // Constructor
    Settings() = default;

    // Load settings from the JSON file
    bool getSetting();
    bool createSettingsFile();

    // Getter functions to access private member variables
    std::string getBotToken() const { return botToken; }
    int64_t getChatId() const { return chatId; }
    std::string getAppVersion() const { return app_version; }
    std::string getNodeName() const { return node_name; }
    int getCpuCheckDuration() const { return cpuCheckDuration; }
    int getMemoryCheckDuration() const { return memoryCheckDuration; }
    int getCpuLimit() const { return cpuLimit; }
    int getMemoryLimit() const { return memoryLimit; }
    std::vector<NodeStructure> getNodeList() const { return node_list; }
    bool getDefaultMonitoringStatus() const { return defaultMonitoringStatus; }

private:
    // settings parameters
    std::string botToken;
    int64_t chatId;
    std::string app_version;
    std::string node_name;
    int cpuCheckDuration;
    int memoryCheckDuration;
    int cpuLimit;
    int memoryLimit;
    bool defaultMonitoringStatus;
    std::vector<NodeStructure> node_list;

    // dependencies
    Log Logger;
};
