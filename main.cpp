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

void thread_getCPUUsage();
void thread_getMemoryUsage();
void thread_telegramBot();
void thread_telegramNotification();
bool getSetting();

std::string botToken;
int64_t chatId;

double lastCpuUsage;
double lastMemoryUsage;
int memoryLimit = 0;
int cpuLimit = 2;
int cpuCheckDuration = 500;
int memoryCheckDuration = 500;

using json = nlohmann::json;

int main()
{
    // Get settings from setting file
    if (!getSetting())
    {
        std::cout << "Failed to set settings!";
        return 0;
    }

    // Create a new thread and run `threadFunction` on it
    std::thread cpuMonitorThread(thread_getCPUUsage);
    std::thread memoryMonitorThread(thread_getMemoryUsage);
    std::thread telegramBotHandler(thread_telegramBot);
    std::thread telegramNotificationHandler(thread_telegramNotification);

    // Wait for the thread to finish
    cpuMonitorThread.join();
    memoryMonitorThread.join();
    telegramBotHandler.join();
    telegramNotificationHandler.join();

    return 0;
}

void thread_getCPUUsage()
{
    while (true)
    {
        long long user1, nice1, system1, idle1;
        long long user2, nice2, system2, idle2;

        // Helper function to read CPU times from /proc/stat
        auto readCpuTimes = [](long long &user, long long &nice, long long &system, long long &idle)
        {
            std::ifstream statFile("/proc/stat");
            if (!statFile.is_open())
            {
                std::cerr << "Error opening /proc/stat" << std::endl;
                exit(1);
            }

            std::string line;
            std::getline(statFile, line); // Read the first line (the "cpu" line)
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel; // Skip the "cpu" label
            iss >> user >> nice >> system >> idle;

            statFile.close();
        };

        // Read initial CPU times
        readCpuTimes(user1, nice1, system1, idle1);

        // Sleep for a while to get a comparison period
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Read CPU times again after the sleep
        readCpuTimes(user2, nice2, system2, idle2);

        // Calculate differences
        long long total1 = user1 + nice1 + system1 + idle1;
        long long total2 = user2 + nice2 + system2 + idle2;

        long long totalDiff = total2 - total1;
        long long idleDiff = idle2 - idle1;

        // Calculate CPU usage percentage
        double cpuUsage = 100.0 * (totalDiff - idleDiff) / totalDiff;

        // Update last usage
        lastCpuUsage = cpuUsage;

        // Sleep for 1 second before the next measurement
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
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
            std::cerr << "Total memory not found in /proc/meminfo" << std::endl;
            return;
        }

        // Calculate used memory and memory usage percentage
        long long usedMemory = totalMemory - freeMemory;
        double memoryUsagePercent = 100.0 * usedMemory / totalMemory;

        // Update memory usage
        lastMemoryUsage = memoryUsagePercent;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void thread_telegramBot()
{
    TgBot::Bot bot(botToken);

    // On start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=chatId){
                                    return;
                                }
                                bot.getApi().sendMessage(message->chat->id, "Welcome to LinuxMonitoring\nCommands:\n/usage     get server status"); });

    // On usage
    bot.getEvents().onCommand("usage", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=chatId){
                                    return;
                                }
                                bot.getApi().sendMessage(message->chat->id, "CPU : " + std::to_string((int)lastCpuUsage )+ "%\nMemory : " + std::to_string((int)lastMemoryUsage) + "%"); });

    // bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message)
    //                              {
    //     printf("User wrote %s\n", message->text.c_str());
    //     if (StringTools::startsWith(message->text, "/start")) {
    //         return;
    //     }
    //     bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text); });

    try
    {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true)
        {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException &e)
    {
        printf("error: %s\n", e.what());
    }
}

void thread_telegramNotification()
{
    TgBot::Bot bot(botToken);

    // Check usage with limit
    while (true)
    {
        // Check cpu limit
        if (cpuLimit != 0 && cpuLimit > 0 && lastCpuUsage >= cpuLimit)
        {
            bot.getApi().sendMessage(chatId, "CPU Warning!\nCpu : " + std::to_string((int)lastCpuUsage) + "%");
        }

        // Check memory limit
        if (memoryLimit != 0 && memoryLimit > 0 && lastMemoryUsage >= memoryLimit)
        {
            bot.getApi().sendMessage(chatId, "Memory Warning!\nMemory : " + std::to_string((int)lastMemoryUsage) + "%");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool getSetting()
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