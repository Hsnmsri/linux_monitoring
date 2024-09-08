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

void thread_getCPUUsage();
void thread_getMemoryUsage();
void thread_telegramBot();
void onStart(TgBot::Message::Ptr message);

std::string botToken = "7427461633:AAGgMN4Km9mm2VGigT2p9kLRnG4kLh22hBk";
const int64_t chatId = 6305232259;

double lastCpuUsage;
double lastMemoryUsage;

int main()
{
    // Create a new thread and run `threadFunction` on it
    std::thread cpuMonitorThread(thread_getCPUUsage);
    std::thread memoryMonitorThread(thread_getMemoryUsage);
    std::thread telegramBotHandler(thread_telegramBot);

    // Wait for the thread to finish
    cpuMonitorThread.join();
    memoryMonitorThread.join();
    telegramBotHandler.join();

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
    bot.getEvents().onCommand("usage", [&bot](TgBot::Message::Ptr message)
                              { 
                                if(message->chat->id!=chatId){
                                    return;
                                }
                                bot.getApi().sendMessage(message->chat->id, "CPU : " + std::to_string((int)lastCpuUsage )+ "%\n Memory : " + std::to_string((int)lastMemoryUsage) + "%"); });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message)
                                 {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text); });

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
