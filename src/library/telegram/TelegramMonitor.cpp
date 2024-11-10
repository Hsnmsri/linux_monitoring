#include "TelegramMonitor.hpp"

TelegramMonitor::TelegramMonitor(bool &isMonitoringEnable, CpuMonitor &cpu, MemoryMonitor &memory, const Settings settings, Log logger)
    : isMonitoringEnable(isMonitoringEnable), cpu(cpu), memory(memory), settings(settings), logger(logger), tgNotificationStatus(false), bot(settings.getBotToken())
{
}

/**
 * @brief Starts a new thread to handle Telegram bot requests.
 *
 * This method creates and starts a new thread to handle incoming Telegram bot commands and
 * interactions. The thread runs the `thread_telegramBot` method, which is responsible for
 * processing Telegram bot commands such as `/start`, `/stop`, `/status`, etc. This method
 * detaches the thread to allow it to run independently in the background.
 *
 * The steps performed by this method are:
 *
 * 1. **Thread Creation**: Creates a new `std::thread` object that executes the `thread_telegramBot`
 *    method of the `TelegramMonitor` class. The `this` pointer is passed to the method to allow
 *    access to the instance variables and methods.
 *
 * 2. **Thread Detachment**: Calls the `detach` method on the `std::thread` object to allow the
 *    thread to run in the background independently of the main thread. Detaching the thread
 *    means that it will not be joined or synchronized with the main thread, and its execution
 *    will continue as long as needed.
 *
 * **Note**: Since the thread is detached, ensure that all necessary synchronization mechanisms
 * (such as mutexes) are in place to handle concurrent access to shared resources.
 *
 * Example usage:
 * @code
 * TelegramMonitor monitor;
 * monitor.startTelegramRequestThread();
 * // The botRequestThread will now be running and processing bot commands in the background.
 * @endcode
 */
void TelegramMonitor::startTelegramRequestThread()
{
    botRequestThread = std::thread(&TelegramMonitor::thread_telegramBot, this);
    botRequestThread.detach();
}

/**
 * @brief Starts a new thread to monitor Telegram notifications.
 *
 * This method creates and starts a new thread dedicated to monitoring and sending Telegram
 * notifications based on system metrics (such as CPU and memory usage). The thread executes
 * the `thread_telegramNotification` method, which is responsible for periodically checking
 * system status and sending notifications if certain thresholds are exceeded. The method
 * detaches the thread to allow it to run independently in the background.
 *
 * The steps performed by this method are:
 *
 * 1. **Thread Creation**: Creates a new `std::thread` object that runs the `thread_telegramNotification`
 *    method of the `TelegramMonitor` class. The `this` pointer is passed to the method, allowing
 *    it to access instance variables and methods.
 *
 * 2. **Thread Detachment**: Calls the `detach` method on the `std::thread` object. This allows the
 *    thread to execute independently and continue running in the background, without blocking
 *    the main thread. Detached threads run in parallel and are not synchronized with the main
 *    thread.
 *
 * **Note**: Since the thread is detached, ensure that all shared resources are properly synchronized
 * to avoid race conditions and data inconsistency.
 *
 * Example usage:
 * @code
 * TelegramMonitor monitor;
 * monitor.startTelegramNotificationWatchThread();
 * // The notificationThread will now be running and handling system notifications in the background.
 * @endcode
 */
void TelegramMonitor::startTelegramNotificationWatchThread()
{
    // if monitoring enabled
    if (this->tgNotificationStatus)
    {
        return;
    }

    // if monitoring disabled
    this->tgNotificationStatus = true;
    notificationThread = std::thread(&TelegramMonitor::thread_telegramNotification, this);
    notificationThread.detach();
}

/**
 * @brief Handles the /start command to initiate monitoring.
 *
 * Checks if the user ID matches the allowed chat ID, logs the start command,
 * enables monitoring, and sends a welcome message with available commands to the user.
 *
 * @param message Pointer to the incoming message containing the /start command.
 */
void TelegramMonitor::handleStartCommand(TgBot::Message::Ptr message)
{
    if (message->chat->id != settings.getChatId())
        return;

    logger.logToConsole("send /start command, start monitoring");
    this->isMonitoringEnable = true;

    bot.getApi().sendMessage(message->chat->id,
                             "Welcome to LinuxMonitoring\n"
                             "\nCommands:\n"
                             "/start    start monitoring\n"
                             "/stop     stop monitoring\n"
                             "/status   monitoring status\n"
                             "/usage    get server status\n"
                             "/help     get bot command list\n"
                             "\nMonitoring Status : Enable\n"
                             "\nPowered By Mr.Mansouri");
}

/**
 * @brief Handles the /stop command to disable monitoring.
 *
 * Checks if the user ID matches the allowed chat ID, logs the stop command,
 * disables monitoring, and notifies the user that monitoring has been stopped.
 *
 * @param message Pointer to the incoming message containing the /stop command.
 */
void TelegramMonitor::handleStopCommand(TgBot::Message::Ptr message)
{
    if (message->chat->id != settings.getChatId())
        return;

    logger.logToConsole("send /stop command, stop monitoring");
    this->isMonitoringEnable = false;

    bot.getApi().sendMessage(message->chat->id,
                             "Monitoring Stopped!\n"
                             "\nMonitoring Status : Disable\n"
                             "\n- To re-enable monitoring, please enter the /start command.\n"
                             "\n- To check monitoring status, please enter the /status command.\n");
}

/**
 * @brief Handles the /usage command to report server CPU and memory usage.
 *
 * Checks if the user ID matches the allowed chat ID and logs the usage command.
 * If monitoring is enabled, sends the current CPU and memory usage to the user.
 * If monitoring is disabled, informs the user that monitoring is inactive.
 *
 * @param message Pointer to the incoming message containing the /usage command.
 */
void TelegramMonitor::handleUsageCommand(TgBot::Message::Ptr message)
{
    if (message->chat->id != settings.getChatId())
        return;

    logger.logToConsole("send /usage command");

    if (!this->isMonitoringEnable)
    {
        bot.getApi().sendMessage(message->chat->id,
                                 "Monitoring Status : Disable\n"
                                 "\nTo monitor the server again, please enter the /start command.");
        return;
    }

    bot.getApi().sendMessage(message->chat->id,
                             "Server Usage :\n\n"
                             "CPU : " +
                                 std::to_string(static_cast<int>(cpu.getLastCpuUsage())) +
                                 "%\nMemory : " + std::to_string(static_cast<int>(memory.getLastMemoryUsage())) + "%");
}

/**
 * @brief Handles the /help command to display available bot commands.
 *
 * Checks if the user ID matches the allowed chat ID and sends a list of all available
 * bot commands to the user, explaining their usage.
 *
 * @param message Pointer to the incoming message containing the /help command.
 */
void TelegramMonitor::handleHelpCommand(TgBot::Message::Ptr message)
{
    if (message->chat->id != settings.getChatId())
        return;

    bot.getApi().sendMessage(message->chat->id,
                             "Commands:\n\n"
                             "/start    start server monitoring\n"
                             "/stop     stop server monitoring\n"
                             "/status   get server monitoring status\n"
                             "/usage    get server usage\n");
}

/**
 * @brief Handles the /status command to report monitoring status.
 *
 * Checks if the user ID matches the allowed chat ID and sends the current
 * monitoring status (enabled or disabled) to the user.
 *
 * @param message Pointer to the incoming message containing the /status command.
 */
void TelegramMonitor::handleStatusCommand(TgBot::Message::Ptr message)
{
    if (message->chat->id != settings.getChatId())
        return;

    std::string statusString = this->isMonitoringEnable ? "Enable" : "Disable";
    bot.getApi().sendMessage(message->chat->id,
                             "Monitoring Status : " + statusString + "\n"
                                                                     "\n/start    start server monitoring\n"
                                                                     "/stop     stop server monitoring\n");
}

/**
 * @brief Initializes Telegram bot commands and enters a long polling loop.
 *
 * Registers handlers for various bot commands (/start, /stop, /usage, /help, /status)
 * by associating each command with its respective function. Then, starts a long polling
 * loop to keep the bot actively processing incoming messages and commands.
 *
 * This function also outputs bot and user details (username, chat ID, API token)
 * for debugging purposes.
 */
void TelegramMonitor::thread_telegramBot()
{
    bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message)
                              { handleStartCommand(message); });
    bot.getEvents().onCommand("stop", [this](TgBot::Message::Ptr message)
                              { handleStopCommand(message); });
    bot.getEvents().onCommand("usage", [this](TgBot::Message::Ptr message)
                              { handleUsageCommand(message); });
    bot.getEvents().onCommand("help", [this](TgBot::Message::Ptr message)
                              { handleHelpCommand(message); });
    bot.getEvents().onCommand("status", [this](TgBot::Message::Ptr message)
                              { handleStatusCommand(message); });

    try
    {
        std::cout << "Bot Username : " << bot.getApi().getMe()->username.c_str() << std::endl;
        std::cout << "User ChatID : " << settings.getChatId() << std::endl;
        std::cout << "Bot API Token : " << settings.getBotToken() << std::endl;

        TgBot::TgLongPoll longPoll(bot);
        while (true)
        {
            longPoll.start();
        }
    }
    catch (TgBot::TgException &e)
    {
        std::cerr << "error : " << e.what() << std::endl;
    }
}

/**
 * @brief Periodically checks CPU and memory usage and sends notifications if usage exceeds predefined limits.
 *
 * This method initializes a Telegram bot and continuously monitors the CPU and memory usage
 * of the system. It compares the current usage against predefined limits specified in the settings.
 * If the CPU or memory usage exceeds the respective limit, it sends a warning message to the
 * designated Telegram chat.
 *
 * The method operates as follows:
 *
 * 1. **Bot Initialization**: Creates an instance of `TgBot::Bot` using the bot token retrieved
 *    from settings.
 *
 * 2. **Monitoring Loop**:
 *    - The loop runs indefinitely, continuously checking system resource usage.
 *    - **Monitoring Check**: If monitoring is disabled (i.e., `isMonitoringEnable` is false),
 *      the thread sleeps for a short period (1 second) and then continues to the next iteration.
 *
 * 3. **CPU Usage Check**:
 *    - If a CPU usage limit is defined and the current CPU usage exceeds this limit, a log message
 *      is recorded and a warning message is sent to the designated chat.
 *
 * 4. **Memory Usage Check**:
 *    - If a memory usage limit is defined and the current memory usage exceeds this limit, a log
 *      message is recorded and a warning message is sent to the designated chat.
 *
 * 5. **Delay**:
 *    - The thread sleeps for a short period (500 milliseconds) before performing the next check.
 *
 * **Note**: Ensure that the `settings.getChatId()`, `settings.getCpuLimit()`, and
 * `settings.getMemoryLimit()` methods return valid values for the bot to function properly.
 * The bot must have sufficient permissions to send messages to the specified chat.
 *
 * Example usage:
 * @code
 * TelegramMonitor monitor;
 * monitor.startTelegramNotificationWatchThread();
 * // The bot will now monitor CPU and memory usage and send notifications as needed.
 * @endcode
 */
void TelegramMonitor::thread_telegramNotification()
{
    TgBot::Bot bot(settings.getBotToken());

    // Check usage with limit
    while (this->tgNotificationStatus)
    {
        // Check cpu limit
        if (settings.getCpuLimit() != 0 && settings.getCpuLimit() > 0 && cpu.getLastCpuUsage() >= settings.getCpuLimit())
        {
            logger.logToConsole("cpu overload (" + std::to_string((int)cpu.getLastCpuUsage()) + "%)");
            bot.getApi().sendMessage(settings.getChatId(), "CPU Warning!\nCpu : " + std::to_string((int)cpu.getLastCpuUsage()) + "%");
        }

        // Check memory limit
        if (settings.getMemoryLimit() != 0 && settings.getMemoryLimit() > 0 && memory.getLastMemoryUsage() >= settings.getMemoryLimit())
        {
            logger.logToConsole("memory overload (" + std::to_string((int)memory.getLastMemoryUsage()) + "%)");
            bot.getApi().sendMessage(settings.getChatId(), "Memory Warning!\nMemory : " + std::to_string((int)memory.getLastMemoryUsage()) + "%");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/**
 * @brief Stops the Telegram notification watching thread.
 *
 * This function sets the `tgNotificationStatus` flag to `false`,
 * indicating that the monitoring of Telegram notifications should
 * cease. As a result, the associated thread responsible for
 * observing notifications will be terminated, and the system will
 * no longer process incoming Telegram messages for monitoring.
 *
 * It is important to call this method when notification monitoring
 * is no longer needed to ensure proper resource management and
 * graceful termination of the associated thread.
 */
void TelegramMonitor::stopTelegramNotificationWatchThread()
{
    this->tgNotificationStatus = false;
}
