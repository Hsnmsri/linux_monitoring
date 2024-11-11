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
    node_name = settings["node_name"];
    cpuCheckDuration = (int)settings["cpu_check_duration"];
    memoryCheckDuration = (int)settings["memory_check_duration"];
    cpuLimit = (int)settings["cpu_limit"];
    memoryLimit = (int)settings["memory_limit"];
    defaultMonitoringStatus = (bool)settings["default_monitoring_status"];

    // parse node_list
    if (settings.contains("node_list") && !settings.is_array())
    {
        for (const auto &node_json : settings["node_list"])
        {
            NodeStructure node;
            node.name = node_json["name"];
            node.ip = node_json["ip"];
            node.port = node_json["port"];
            node.secret = node_json["secret"];
            node_list.push_back(node);
        }
    }
    else
    {
        Logger.logToConsole("Node list is not defined!");
    }

    return true;
}

/**
 * @brief Creates a settings file (settings.json) with application configuration settings provided by the user.
 *
 * This function collects configuration parameters from the user, including:
 * - `bot_token`: Token for the bot integration (non-empty).
 * - `chat_id`: The chat ID associated with the bot.
 * - `node_name`: Identifier for the current node.
 * - `cpu_check_duration`: Duration in milliseconds for CPU monitoring checks.
 * - `memory_check_duration`: Duration in milliseconds for memory monitoring checks.
 * - `cpu_limit`: CPU usage limit as a percentage for monitoring.
 * - `memory_limit`: Memory usage limit as a percentage for monitoring.
 * - `default_monitoring_status`: Boolean indicating whether monitoring is enabled by default.
 *
 * The collected settings are stored in a JSON object, and `node_list` is initialized as an empty array.
 * The function then writes this JSON object to a file named "settings.json" in a human-readable format.
 *
 * @return bool - Returns true if the settings file is created successfully, otherwise returns false if an error occurs during file creation.
 */
bool Settings::createSettingsFile()
{
    json settings;

    // Temporary variables to hold user input
    std::string botToken, appVersion = "1.2.3", nodeName;
    int64_t chatId;
    int cpuCheckDuration, memoryCheckDuration, cpuLimit, memoryLimit;
    bool defaultMonitoringStatus;

    // Set the app version directly
    // Use consistent key name here
    settings["version"] = appVersion;
    settings["node_list"] = nlohmann::json::array();

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

    // Node name
    while (true)
    {
        std::cout << "Enter current node name: ";
        std::cin >> nodeName;
        if (!nodeName.empty())
        {
            settings["node_name"] = nodeName;
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
