#include "TelegramMonitor.hpp"

TelegramMonitor::TelegramMonitor(bool &isMonitoringEnable, CpuMonitor &cpu, MemoryMonitor &memory, const Settings settings, Log logger)
    : isMonitoringEnable(isMonitoringEnable), cpu(cpu), memory(memory), settings(settings), logger(logger) {}

void TelegramMonitor::startTelegramRequestThread()
{
    botRequestThread = std::thread(&TelegramMonitor::thread_telegramBot, this);
    botRequestThread.detach();
}

void TelegramMonitor::startTelegramNotificationWatchThread()
{
    notificationThread = std::thread(&TelegramMonitor::thread_telegramNotification, this);
    notificationThread.detach();
}

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
