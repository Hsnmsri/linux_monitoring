#include "Settings.hpp"

bool Settings::getSetting() {
    // Load the settings.json file
    std::ifstream settings_file("settings.json");
    if (!settings_file) {
        std::cerr << "Error opening settings.json file." << std::endl;
        return false;
    }

    // Parse the JSON file
    json settings;
    settings_file >> settings;

    // Access values from the JSON object
    botToken = settings["bot_token"];
    chatId = (int64_t)settings["chat_id"];
    cpuCheckDuration = (int)settings["cpu_check_duration"];
    memoryCheckDuration = (int)settings["memory_check_duration"];
    cpuLimit = (int)settings["cpu_limit"];
    memoryLimit = (int)settings["memory_limit"];

    return true;
}
