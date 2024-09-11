#include "Settings.hpp"

/**
 * @brief Loads and parses the settings from a JSON file.
 *
 * This method attempts to load and parse configuration settings from the `settings.json`
 * file. It reads various settings related to the bot, monitoring intervals, and thresholds,
 * and stores them in member variables. The method performs the following tasks:
 *
 * 1. **File Opening**: Opens the `settings.json` file for reading. If the file cannot be opened,
 *    an error message is printed, and the method returns `false`.
 *
 * 2. **JSON Parsing**: Parses the JSON content from the file into a `json` object. This requires
 *    the use of a JSON library, such as [nlohmann/json](https://github.com/nlohmann/json).
 *
 * 3. **Setting Extraction**: Extracts specific settings from the JSON object:
 *    - `botToken`: The bot token for Telegram authentication.
 *    - `chatId`: The chat ID used for sending messages.
 *    - `cpuCheckDuration`: The duration (in milliseconds) for checking CPU usage.
 *    - `memoryCheckDuration`: The duration (in milliseconds) for checking memory usage.
 *    - `cpuLimit`: The CPU usage threshold for generating warnings.
 *    - `memoryLimit`: The memory usage threshold for generating warnings.
 *
 * 4. **Return Value**: Returns `true` if the settings were successfully loaded and parsed.
 *    Otherwise, it returns `false` if there was an error opening the file.
 *
 * Example usage:
 * @code
 * Settings settings;
 * if (settings.getSetting()) {
 *     // Settings successfully loaded
 *     // Access settings through member variables
 * } else {
 *     // Handle the error
 * }
 * @endcode
 *
 * @return True if the settings were successfully loaded and parsed; false otherwise.
 */
bool Settings::getSetting()
{
    // Load the settings.json file
    std::ifstream settings_file("settings.json");
    if (!settings_file)
    {
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
