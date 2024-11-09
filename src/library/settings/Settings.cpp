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
    app_version = settings["version"];
    server_name = settings["server_name"];
    cpuCheckDuration = (int)settings["cpu_check_duration"];
    memoryCheckDuration = (int)settings["memory_check_duration"];
    cpuLimit = (int)settings["cpu_limit"];
    memoryLimit = (int)settings["memory_limit"];
    defaultMonitoringStatus = (bool)settings["default_monitoring_status"];

    return true;
}

bool Settings::createSettingsFile()
{
    json settings;

    // Temporary variables to hold user input
    std::string botToken, appVersion = "1.2.1", serverName;
    int64_t chatId;
    int cpuCheckDuration, memoryCheckDuration, cpuLimit, memoryLimit;
    bool defaultMonitoringStatus;

    // Set the app version directly
    settings["version"] = appVersion; // Use consistent key name here

    // Bot token
    while (true)
    {
        std::cout << "Enter bot token: ";
        std::cin >> botToken;
        if (!botToken.empty())
        {
            settings["bot_token"] = botToken;
            break;
        }
        else
        {
            std::cout << "Bot token cannot be empty. Please enter a valid bot token.\n";
        }
    }

    // Chat ID
    while (true)
    {
        std::cout << "Enter chat ID: ";
        if (std::cin >> chatId)
        {
            settings["chat_id"] = chatId;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter a valid chat ID.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Server name
    while (true)
    {
        std::cout << "Enter server name: ";
        std::cin >> serverName;
        if (!serverName.empty())
        {
            settings["server_name"] = serverName;
            break;
        }
        else
        {
            std::cout << "Server name cannot be empty. Please enter a valid server name.\n";
        }
    }

    // CPU check duration
    while (true)
    {
        std::cout << "Enter CPU check duration (ms): ";
        if (std::cin >> cpuCheckDuration)
        {
            settings["cpu_check_duration"] = cpuCheckDuration;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter a valid duration in milliseconds.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Memory check duration
    while (true)
    {
        std::cout << "Enter memory check duration (ms): ";
        if (std::cin >> memoryCheckDuration)
        {
            settings["memory_check_duration"] = memoryCheckDuration;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter a valid duration in milliseconds.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // CPU usage limit
    while (true)
    {
        std::cout << "Enter CPU usage limit (%): ";
        if (std::cin >> cpuLimit)
        {
            settings["cpu_limit"] = cpuLimit;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter a valid percentage.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Memory usage limit
    while (true)
    {
        std::cout << "Enter memory usage limit (%): ";
        if (std::cin >> memoryLimit)
        {
            settings["memory_limit"] = memoryLimit;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter a valid percentage.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Default monitoring status
    while (true)
    {
        std::cout << "Enable default monitoring status? (1 for true, 0 for false): ";
        int monitoringInput;
        if (std::cin >> monitoringInput && (monitoringInput == 0 || monitoringInput == 1))
        {
            defaultMonitoringStatus = static_cast<bool>(monitoringInput);
            settings["default_monitoring_status"] = defaultMonitoringStatus;
            break;
        }
        else
        {
            std::cout << "Invalid input. Please enter 1 for true or 0 for false.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    // Write the JSON object to a file
    std::ofstream settings_file("settings.json");
    if (settings_file)
    {
        settings_file << settings.dump(4); // Pretty-print with indentation
        settings_file.close();
        std::cout << "Settings file created successfully.\n";
        return true;
    }
    else
    {
        std::cerr << "Error creating settings.json file.\n";
        return false;
    }
}
