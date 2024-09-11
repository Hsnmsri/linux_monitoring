#include "TelegramMonitor.hpp"

TelegramMonitor::TelegramMonitor(bool &isMonitoringEnable, CpuMonitor &cpu, MemoryMonitor &memory, const Settings settings, Log logger)
    : isMonitoringEnable(isMonitoringEnable), cpu(cpu), memory(memory), settings(settings), logger(logger) {}

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
    notificationThread = std::thread(&TelegramMonitor::thread_telegramNotification, this);
    notificationThread.detach();
}

/**
 * @brief Handles incoming Telegram bot commands and interacts with users.
 *
 * This method initializes a Telegram bot and sets up event handlers for various bot commands.
 * The bot listens for commands such as /start, /stop, /usage, /help, and /status. Based on the
 * received command, it performs actions such as starting or stopping monitoring, sending server
 * usage statistics, and providing help or status information.
 *
 * The method operates as follows:
 *
 * 1. **Bot Initialization**: Creates an instance of `TgBot::Bot` using the bot token retrieved
 *    from settings.
 *
 * 2. **Command Handlers**:
 *    - **start**: Enables monitoring if the chat ID matches the expected ID, logs the event,
 *      and sends a welcome message with a list of available commands. Updates the monitoring
 *      status to "Enable".
 *    - **stop**: Disables monitoring if the chat ID matches, logs the event, and sends a
 *      message indicating that monitoring has stopped. Updates the monitoring status to "Disable".
 *    - **usage**: Sends current server usage statistics (CPU and memory) if monitoring is enabled.
 *      If monitoring is disabled, it sends a message indicating how to re-enable monitoring.
 *    - **help**: Sends a message listing available commands to help the user interact with the bot.
 *    - **status**: Sends the current monitoring status ("Enable" or "Disable") along with
 *      instructions for starting or stopping monitoring.
 *
 * 3. **Bot Polling**: Sets up a `TgBot::TgLongPoll` object for long polling to receive updates
 *    from the Telegram server. Enters an infinite loop, continuously calling `longPoll.start()`
 *    to process incoming messages and commands.
 *
 * 4. **Error Handling**: Catches and logs any exceptions thrown by the Telegram Bot API, ensuring
 *    that errors are reported for debugging purposes.
 *
 * **Note**: Ensure that the `settings.getChatId()` and `settings.getBotToken()` methods return
 * correct values for the bot to function properly. The bot must be properly configured in Telegram
 * and have the necessary permissions to interact with users.
 *
 * Example usage:
 * @code
 * TelegramMonitor monitor;
 *monitor.startTelegramBot();
 * // The bot will now be running and handling commands.
 *@endcode
 **/
void TelegramMonitor::thread_telegramBot()
{
    TgBot::Bot bot(settings.getBotToken());

    // command Start
    bot.getEvents().onCommand("start", [&bot, this](TgBot::Message::Ptr message)
                              {
        // check user id
        if (message->chat->id != settings.getChatId()) return;

        // log event
        logger.logToConsole("send /start command,start monitoring");

        // enable monitoring
        this->isMonitoringEnable=true;

        // send user message
        bot.getApi().sendMessage(message->chat->id, "Welcome to LinuxMonitoring\n"
        "\nCommands:\n"
        "/start    start monitoring\n"
        "/stop     stop monitoring\n"
        "/status    monitoring status\n"
        "/usage    get server status\n"
        "/help     get bot command list\n"
        "\nMonitoring Status : Enable\n"
        "\nPowered By Mr.Mansouri"); });

    // command Stop
    bot.getEvents().onCommand("stop", [&bot, this](TgBot::Message::Ptr message)
                              {
        // check user id
        if (message->chat->id != settings.getChatId())return;

        // log event
        logger.logToConsole("send /stop command,stop monitoring");

        // disbale monitoring
        this->isMonitoringEnable=false;

        // send message
        bot.getApi().sendMessage(message->chat->id, 
        "Monitoring Stopped!\n"
        "\nMonitoring Status : Disable\n"
        "\n- To re-enable monitoring, please enter the /start command.\n"
        "\n- To check monitoring status, please enter the /status command.\n"
        ); });

    // command Usage
    bot.getEvents().onCommand("usage", [&bot, this](TgBot::Message::Ptr message)
                              {
        // check user id
        if (message->chat->id != settings.getChatId()) return;

        // log event
        logger.logToConsole("send /usage command");

        // if monitoring disable
        if(!this->isMonitoringEnable){
            bot.getApi().sendMessage(message->chat->id,
            "Monitoring Status : Disable\n"
            "\nTo monitor the server again, please enter the /start command."
            );
            return;
        }

        // send message
        bot.getApi().sendMessage(message->chat->id, 
        "Server Usage :\n\n"
        "CPU : " + std::to_string((int)cpu.getLastCpuUsage()) + 
        "%\nMemory : " + std::to_string((int)memory.getLastMemoryUsage()) + "%"
        ); });

    // command Help
    bot.getEvents().onCommand("help", [&bot, this](TgBot::Message::Ptr message)
                              {
        // check user id
        if (message->chat->id != settings.getChatId()) return;
        
        // send message
        bot.getApi().sendMessage(message->chat->id, 
        "Commands:\n\n"
        "/start    start server monitoring\n"
        "/stop     stop server monitoring\n"
        "/status   get server monitoring status\n"
        "/usage    get server usage\n"
        ); });

    // command Status
    bot.getEvents().onCommand("status", [&bot, this](TgBot::Message::Ptr message)
                              {
        // check user id
        if (message->chat->id != settings.getChatId()) return;
        
        // send message
        std::string statusString;
        if(this->isMonitoringEnable){
            statusString="Enable";
        }else{
            statusString="Disable";
        }
        bot.getApi().sendMessage(message->chat->id, 
        "Monitoring Status : " + statusString + "\n"
        "\n/start    start server monitoring\n"
        "/stop    stop server monitoring\n"
        ); });

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
    while (true)
    {
        // if monitoring disable
        if (!this->isMonitoringEnable)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

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
