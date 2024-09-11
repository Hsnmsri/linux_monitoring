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

    // On start
    bot.getEvents().onCommand("start", [&bot, this](TgBot::Message::Ptr message)
                              {
        if (message->chat->id != settings.getChatId()) {
            return;
        }
        logger.logToConsole("send /start command");
        bot.getApi().sendMessage(message->chat->id, "Welcome to LinuxMonitoring\n\nCommands:\n/usage     get server status\n/help     get bot command list \n\nPowered By Mr.Mansouri"); });

    // On usage
    bot.getEvents().onCommand("usage", [&bot, this](TgBot::Message::Ptr message)
                              {
        if (message->chat->id != settings.getChatId()) {
            return;
        }
        logger.logToConsole("send /usage command");
        bot.getApi().sendMessage(message->chat->id, "CPU : " + std::to_string((int)cpu.getLastCpuUsage()) + "%\nMemory : " + std::to_string((int)memory.getLastMemoryUsage()) + "%"); });

    // On usage
    bot.getEvents().onCommand("stop", [&bot, this](TgBot::Message::Ptr message)
                              {
        if (message->chat->id != settings.getChatId()) {
            return;
        }
        this->isMonitoringEnable=false;
        logger.logToConsole("send /stop command");
        bot.getApi().sendMessage(message->chat->id, "App Stopped!"); });

    // On help
    bot.getEvents().onCommand("help", [&bot, this](TgBot::Message::Ptr message)
                              {
        if (message->chat->id != settings.getChatId()) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Commands:\n/usage     get server status"); });

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
