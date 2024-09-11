#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <curl/curl.h>
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <Log.hpp>
#include <Settings.hpp>
#include <CpuMonitor.hpp>
#include <MemoryMonitor.hpp>

void thread_telegramBot();
void thread_telegramNotification();

Log logger;
Settings settings;

double lastCpuUsage;
double lastMemoryUsage;

using json = nlohmann::json;

int main()
{
    logger.logToConsole("Linux Monitoring Service Started");

    // Get settings from setting file
    if (!settings.getSetting())
    {
        logger.logToConsole("Failed to set settings!");
        return 0;
    }

    // Monitoring CPU
    CpuMonitor cpu(settings.getCpuCheckDuration());
    MemoryMonitor memory(settings.getMemoryCheckDuration());

    cpu.startMonitoring();
    memory.startMonitoring();
    while (true)
    {
        std::cout << memory.getLastMemoryUsage() << std::endl;
    }

    // Create a new thread and run `threadFunction` on it
    std::thread telegramBotHandler(thread_telegramBot);
    std::thread telegramNotificationHandler(thread_telegramNotification);

    // Wait for the thread to finish
    telegramBotHandler.join();
    telegramNotificationHandler.join();

    return 0;
}

void thread_getMemoryUsage()
{
    while (true)
    {
        std::ifstream memInfoFile("/proc/meminfo");
        if (!memInfoFile.is_open())
        {
            std::cerr << "Error opening /proc/meminfo" << std::endl;
            return;
        }

        std::string line;
        long long totalMemory = 0;
        long long freeMemory = 0;

        while (std::getline(memInfoFile, line))
        {
            std::istringstream iss(line);
            std::string key;
            long long value;
            std::string unit;

            iss >> key >> value >> unit;

            if (key == "MemTotal:")
            {
                totalMemory = value;
            }
            if (key == "MemAvailable:")
            {
                freeMemory = value;
            }
        }

        memInfoFile.close();

        if (totalMemory == 0)
        {
            logger.logToConsole("Total memory not found in /proc/meminfo");
            return;
        }

        // Calculate used memory and memory usage percentage
        long long usedMemory = totalMemory - freeMemory;
        double memoryUsagePercent = 100.0 * usedMemory / totalMemory;

        // Update memory usage
        lastMemoryUsage = memoryUsagePercent;

        std::this_thread::sleep_for(std::chrono::milliseconds(settings.getMemoryCheckDuration()));
    }
}

void thread_telegramBot()
{
    TgBot::Bot bot(settings.getBotToken());

    // On start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=settings.getChatId()){
                                    return;
                                }
                                logger.logToConsole("send /start command");
                                bot.getApi().sendMessage(message->chat->id, "Welcome to LinuxMonitoring\n\nCommands:\n/usage     get server status\n/help     get bot command list \n\nPowered By Mr.Mansouri"); });

    // On usage
    bot.getEvents().onCommand("usage", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=settings.getChatId()){
                                    return;
                                }
                                logger.logToConsole("send /usage command");
                                bot.getApi().sendMessage(message->chat->id, "CPU : " + std::to_string((int)lastCpuUsage )+ "%\nMemory : " + std::to_string((int)lastMemoryUsage) + "%"); });

    // On help
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=settings.getChatId()){
                                    return;
                                }
                                bot.getApi().sendMessage(message->chat->id, "Commands:\n/usage     get server status"); });

    // bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message)
    //                              {
    //     printf("User wrote %s\n", message->text.c_str());
    //     if (StringTools::startsWith(message->text, "/start")) {
    //         return;
    //     }
    //     bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text); });

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
        printf("error : %s\n", e.what());
    }
}

void thread_telegramNotification()
{
    TgBot::Bot bot(settings.getBotToken());

    // Check usage with limit
    while (true)
    {
        // Check cpu limit
        if (settings.getCpuLimit() != 0 && settings.getCpuLimit() > 0 && lastCpuUsage >= settings.getCpuLimit())
        {
            logger.logToConsole("cpu overload (" + std::to_string((int)lastCpuUsage) + "%)");
            bot.getApi().sendMessage(settings.getChatId(), "CPU Warning!\nCpu : " + std::to_string((int)lastCpuUsage) + "%");
        }

        // Check memory limit
        if (settings.getMemoryLimit() != 0 && settings.getMemoryLimit() > 0 && lastMemoryUsage >= settings.getMemoryLimit())
        {
            logger.logToConsole("memory overload (" + std::to_string((int)lastMemoryUsage) + "%)");
            bot.getApi().sendMessage(settings.getChatId(), "Memory Warning!\nMemory : " + std::to_string((int)lastMemoryUsage) + "%");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
